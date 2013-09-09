#pragma once

#include <Windows.h>
#include <mmsystem.h>
#include <Mmdeviceapi.h>
#include <Audiopolicy.h>
#include <Endpointvolume.h>
#pragma comment(lib,"winmm.lib")

class CVolume
{
public:
	CVolume()
	{
		// vista及以后的操作系统音量调节方式
		HRESULT hr=CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&m_pMMDeviceEnumerator);
		if(hr != S_OK || m_pMMDeviceEnumerator == NULL)
			return;

		hr = m_pMMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_pMMDevice);
		if(hr != S_OK || m_pMMDevice == NULL)
			return;

		hr = m_pMMDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&m_pIAudioEndpointVolume);
		if(hr != S_OK || m_pIAudioEndpointVolume == NULL)
			return;
	}

public:
	//---------------------------------------------------------------------------
	//得到设备的音量dev=0主音量，WAVE ,2MIDI ,3 CD
	int WINAPI GetVolume(int dev = 0)
	{
		if(m_pIAudioEndpointVolume != NULL)
		{
			// vista及以后的操作系统音量调节方式
			float volume = 0;
			m_pIAudioEndpointVolume->GetMasterVolumeLevelScalar(&volume);
			return volume * 100;
		}

		long device = 0;
		unsigned rt = 0;
		HMIXER hmixer = NULL;
		MIXERCONTROL volCtrl = {0};

		switch (dev)
		{
		case 1:

			device = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
			break;

		case 2:
			device = MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER;
			break;

		case 3:
			device = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;
			break;// cd 音量

		case 4:
			device = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;
			break;//麦克风音量

		case 5:
			device = MIXERLINE_COMPONENTTYPE_SRC_LINE;
			break;//PC 扬声器音量

		default:
			device = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
		}

		if(mixerOpen(&hmixer, 0, 0, 0, 0))
			return 0;

		if(!GetVolumeControl(hmixer, device, MIXERCONTROL_CONTROLTYPE_VOLUME, &volCtrl))
			return 0;

		rt = GetVolumeValue(hmixer, &volCtrl) * 100 / volCtrl.Bounds.lMaximum;
		mixerClose(hmixer);
		return rt;
	}

	//---------------------------------------------------------------------------
	//设置设备的音量
	//dev =0,1,2 分别表示主音量,波形,MIDI ,CD
	//vol=0-100表示音量的大小, 设置与返回音量的值用的是百分比，即音量从-100，而不是设备的绝对值
	//retrun false 表示设置音量的大小的操作不成功
	//retrun true 表示设置音量的大小的操作成功
	bool WINAPI SetVolume(int vol, long dev = 0)
	{
		if(vol < 0)
			vol = 0;

		if(vol > 100)
			vol = 100;

		if(m_pIAudioEndpointVolume != NULL)
		{
			// vista及以后的操作系统音量调节方式
			m_pIAudioEndpointVolume->SetMasterVolumeLevelScalar(vol / 100.0, NULL);
			return true;
		}

		long device;
		bool rc=false;
		MIXERCONTROL volCtrl;
		HMIXER hmixer;

		switch (dev)
		{
		case 1:
			device = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
			break;

		case 2:
			device = MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER;
			break;

		case 3:
			device = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;
			break;// cd 音量

		case 4:
			device = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;
			break;//麦克风音量

		case 5:
			device = MIXERLINE_COMPONENTTYPE_SRC_LINE; break;
			//PC 扬声器音量

		default:
			device = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
		}

		if(mixerOpen(&hmixer, 0, 0, 0, 0))
			return 0;

		if(GetVolumeControl(hmixer, device, MIXERCONTROL_CONTROLTYPE_VOLUME, &volCtrl))
		{
			vol = vol * volCtrl.Bounds.lMaximum / 100;
			if(SetVolumeValue(hmixer, &volCtrl, vol))
				rc = true;
		}

		mixerClose(hmixer);
		return rc;
	}

protected:
	bool GetVolumeControl( HMIXER hmixer ,long componentType,long ctrlType,MIXERCONTROL* mxc)
	{
		MIXERLINECONTROLS mxlc;
		MIXERLINE mxl;
		mxl.cbStruct = sizeof(mxl);
		mxl.dwComponentType = componentType;

		if(!mixerGetLineInfo((HMIXEROBJ)hmixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE))
		{
			mxlc.cbStruct = sizeof(mxlc);
			mxlc.dwLineID = mxl.dwLineID;
			mxlc.dwControlType = ctrlType;
			mxlc.cControls = 1;
			mxlc.cbmxctrl = sizeof(MIXERCONTROL);
			mxlc.pamxctrl = mxc;
			if(mixerGetLineControls((HMIXEROBJ)hmixer,&mxlc,MIXER_GETLINECONTROLSF_ONEBYTYPE))
				return false;
			else
				return true;
		}

		return false;
	}

	//--------------------------------------------------------------------------
	unsigned GetVolumeValue( HMIXER hmixer, MIXERCONTROL *mxc)
	{
		MIXERCONTROLDETAILS mxcd;
		MIXERCONTROLDETAILS_UNSIGNED vol; vol.dwValue=0;
		mxcd.hwndOwner = 0;
		mxcd.cbStruct = sizeof(mxcd);
		mxcd.dwControlID = mxc->dwControlID;
		mxcd.cbDetails = sizeof(vol);
		mxcd.paDetails = &vol;
		mxcd.cChannels = 1;

		if(mixerGetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER|MIXER_GETCONTROLDETAILSF_VALUE))
			return -1;

		return vol.dwValue;
	}

	//---------------------------------------------------------------------------
	bool SetVolumeValue( HMIXER hmixer, MIXERCONTROL *mxc, long volume)
	{
		MIXERCONTROLDETAILS mxcd;
		MIXERCONTROLDETAILS_UNSIGNED vol;vol.dwValue = volume;
		mxcd.hwndOwner = 0;
		mxcd.dwControlID = mxc->dwControlID;
		mxcd.cbStruct = sizeof(mxcd);
		mxcd.cbDetails = sizeof(vol);
		mxcd.paDetails = &vol;
		mxcd.cChannels = 1;

		if(mixerSetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER|MIXER_SETCONTROLDETAILSF_VALUE))
			return false;

		return true;
	}

protected:
	CComPtr<IMMDevice> m_pMMDevice;
	CComPtr<IMMDeviceEnumerator> m_pMMDeviceEnumerator;
	CComPtr<IAudioEndpointVolume> m_pIAudioEndpointVolume;
};
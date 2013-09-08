#pragma once

#include <Windows.h>
#include <mmsystem.h>
#include <Mmdeviceapi.h>
#include <Endpointvolume.h>
#pragma comment(lib,"winmm.lib")

class CVolume
{
public:
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

	//---------------------------------------------------------------------------
	//�õ��豸������dev=0��������WAVE ,2MIDI ,3 CD
	unsigned WINAPI GetVolume(int dev)
	{
		long device;unsigned rt=0;
		MIXERCONTROL volCtrl;
		HMIXER hmixer;

		switch (dev)
		{
		case 1:

			device=MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
			break;

		case 2:
			device=MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER;
			break;

		case 3:
			device=MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;
			break;// cd ����

		case 4:
			device=MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;
			break;//��˷�����

		case 5:
			device=MIXERLINE_COMPONENTTYPE_SRC_LINE;
			break;//PC ����������

		default:
			device=MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
		}

		if(mixerOpen(&hmixer, 0, 0, 0, 0))
			return 0;

		if(!GetVolumeControl(hmixer,device,MIXERCONTROL_CONTROLTYPE_VOLUME,&volCtrl))
			return 0;

		rt=GetVolumeValue(hmixer,&volCtrl)*100/volCtrl.Bounds.lMaximum;
		mixerClose(hmixer);
		return rt;
	}

	//---------------------------------------------------------------------------
	//�����豸������
	//dev =0,1,2 �ֱ��ʾ������,����,MIDI ,CD
	//vol=0-100��ʾ�����Ĵ�С, �����뷵��������ֵ�õ��ǰٷֱȣ���������-100���������豸�ľ���ֵ
	//retrun false ��ʾ���������Ĵ�С�Ĳ������ɹ�
	//retrun true ��ʾ���������Ĵ�С�Ĳ����ɹ�
	bool WINAPI SetVolume( long dev, long vol)
	{
		long device;
		bool rc=false;
		MIXERCONTROL volCtrl;
		HMIXER hmixer;

		switch (dev)
		{
		case 1:
			device=MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
			break;

		case 2:
			device=MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER;
			break;

		case 3:
			device=MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;
			break;// cd ����

		case 4:
			device=MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;
			break;//��˷�����

		case 5:
			device=MIXERLINE_COMPONENTTYPE_SRC_LINE; break;
			//PC ����������

		default:
			device=MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
		}

		if(mixerOpen(&hmixer, 0, 0, 0, 0))
			return 0;

		if(GetVolumeControl(hmixer,device,MIXERCONTROL_CONTROLTYPE_VOLUME,&volCtrl))
		{
			vol=vol*volCtrl.Bounds.lMaximum/100;
			if(SetVolumeValue(hmixer,&volCtrl,vol))
				rc=true;
		}

		mixerClose(hmixer);
		return rc;
	}

	void GetVolumeWin7()
	{
		//vista��win7����ȡ��ͬ�ķ�ʽ��ȡ����  
		IMMDeviceEnumerator* pEnumerator;  
		IMMDeviceCollection* pCollection = NULL;  
		IMMDevice *pDevice = NULL;  
		IPropertyStore *pProperties=NULL;    
		IAudioEndpointVolume *pVolumeAPI=NULL;    
		UINT deviceCount = 0;  

		float fVolume = -1;  

		CoInitializeEx( NULL , COINIT_MULTITHREADED );  

		HRESULT hr=CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&pEnumerator);  
		if (hr != S_OK)    
		{    
			printf("CoCreateInstance Failed!\n");    
			return 0;    
		}    

		//hr = pEnumerator->EnumAudioEndpoints(eCapture /*eRender*/, DEVICE_STATE_ACTIVE, &pCollection);  
		hr = pEnumerator->EnumAudioEndpoints( eRender , DEVICE_STATE_ACTIVE , &pCollection );  
		if (hr != S_OK)    
		{    
			printf("EnumAudioEndpoints Failed!\n");    
			goto releasepEnumerator;    
		}    

		hr = pCollection->GetCount(&deviceCount);    
		if (hr != S_OK)    
		{    
			printf("GetCount Failed!\n");    
			goto releasepCollection;    
		}    

		for (UINT dev=0;dev<deviceCount;dev++)    
		{    
			pDevice = NULL;    
			hr = pCollection->Item(dev,&pDevice);    
			if (hr == S_OK)    
			{    

				hr = pDevice->OpenPropertyStore(STGM_READ,&pProperties);    
				if (hr == S_OK)    
				{    
					PROPVARIANT varName;    
					PropVariantInit(&varName);    
					hr = pProperties->GetValue(PKEY_Device_FriendlyName, &varName);   
					if( SUCCEEDED(hr) )  
					{  
						hr=pDevice->Activate(__uuidof(IAudioEndpointVolume),CLSCTX_ALL,NULL,(void **)(&pVolumeAPI));    
						if (hr==S_OK)    
						{    
							float volumnLevel = (float)volumnIndex /100;  
							hr = pVolumeAPI->SetMasterVolumeLevelScalar( volumnLevel, NULL );  
						}   
					}  
					SAFE_RELEASE(pProperties);    

				}    

				SAFE_RELEASE(pDevice);    
			}    
		}    

releasepCollection:    

		SAFE_RELEASE(pCollection);    

releasepEnumerator:    

		SAFE_RELEASE(pEnumerator);    

		return  fVolume*100;  
	}
};
function media_info()
{
};

media_info.prototype.open = function(filepath)
{
	return Boolean(Number(window.call('', 'mediainfo.open', filepath)));
};

media_info.prototype.file_format = function()
{
	return window.call('', 'mediainfo.file_format');
};

media_info.prototype.duration = function()
{
	return window.call('', 'mediainfo.duration');
};

media_info.prototype.video_format = function()
{
	return window.call('', 'mediainfo.video_format');
};

media_info.prototype.vcodec_id = function()
{
	return window.call('', 'mediainfo.vcodec_id');
};

media_info.prototype.vcodec_info = function()
{
	return window.call('', 'mediainfo.vcodec_info');
};

media_info.prototype.vcodec_profile = function()
{
	return window.call('', 'mediainfo.vcodec_profile');
};

media_info.prototype.vbitrate = function()
{
	return window.call('', 'mediainfo.vbitrate');
};

media_info.prototype.resolution = function()
{
	return window.call('', 'mediainfo.resolution');
};

media_info.prototype.aspect_ratio = function()
{
	return window.call('', 'mediainfo.aspect_ratio');
};

media_info.prototype.fps = function()
{
	return window.call('', 'mediainfo.fps');
};

media_info.prototype.audio_format = function()
{
	return window.call('', 'mediainfo.audio_format');
};

media_info.prototype.acodec_id = function()
{
	return window.call('', 'mediainfo.acodec_id');
};

media_info.prototype.acodec_info = function()
{
	return window.call('', 'mediainfo.acodec_info');
};

media_info.prototype.acodec_profile = function()
{
	return window.call('', 'mediainfo.acodec_profile');
};

media_info.prototype.abitrate = function()
{
	return window.call('', 'mediainfo.abitrate');
};

media_info.prototype.channels = function()
{
	return window.call('', 'mediainfo.channels');
};

media_info.prototype.srate = function()
{
	return window.call('', 'mediainfo.srate');
};

media_info.prototype.bit_depth = function()
{
	return window.call('', 'mediainfo.bit_depth');
};

mediainfo = new media_info();
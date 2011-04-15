function player_class()
{
};

player_class.prototype.play = function(filepath)
{
	return window.call('', 'player.play', filepath);
};

player_class.prototype.pause = function()
{
	return window.call('', 'player.pause');
};

player_class.prototype.stop = function()
{
	return window.call('', 'player.stop');
};

player_class.prototype.mute = function()
{
	return window.call('', 'player.mute');
};

player_class.prototype.set_volume = function(volume)
{
	return window.call('', 'player.SetVolume', volume);
};

player_class.prototype.amplify = function(multiple)
{
	return window.call('', 'player.AmplifyVolume', multiple);
};

player_class.prototype.set_channel = function(channel)
{
	return window.call('', 'player.SetChannel', channel);
};

player_class.prototype.set_video_pos = function(x, y, width, height)
{
	return window.call('', 'player.SetVideoPos', x, y, width, height);
};

player_class.prototype.duration = function()
{
	return Number(window.call('', 'player.GetDuration'));
};

player_class.prototype.get_play_pos = function()
{
	return Number(window.call('', 'player.GetPlayPos'));
};

player_class.prototype.set_play_pos = function(pos)
{
	return window.call('', 'player.SetPlayPos', pos);
};

player_class.prototype.get_fullscreen = function()
{
	return Boolean(Number(window.call('', 'player.GetFullscreen')));
};

player_class.prototype.set_fullscreen = function(fullscreen)
{
	return Boolean(Number(window.call('', 'player.SetFullscreen', fullscreen)));
};

player = new player_class();
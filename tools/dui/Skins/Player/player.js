function player_class()
{
};

player_class.prototype.play = function()
{
	var filepath = dialog.open();
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

player_class.prototype.set_video_pos = function(x, y, width, height)
{
	return window.call('', 'player.SetVideoPos', x, y, width, height);
};

player = new player_class();
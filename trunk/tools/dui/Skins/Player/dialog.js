function dialog_class()
{
};

dialog_class.prototype.open = function()
{
	return window.call('', 'dialog.open', '');
};

dialog_class.prototype.save = function()
{
	return window.call('', 'dialog.save');
};

dialog = new dialog_class();
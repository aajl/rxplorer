function auto_update() {
	var downloaded = Number(sys.call('update.downloaded', 'explorer'));
	if(downloaded) {
		var param = "-wait explorer_plus_direct_ui_mutex -waittime 5000 -start app_d.exe -app -mutex auto_update_mutex -dui Modules/MDirectUI_D.dll -Module Modules/MUpdate_D.dll unused -skin update/skins/skin.xml -appname explorer -localver Skins/version.xml";
		sys.shell_execute(sys.path.app + "/app_d.exe",  param);
		return "exit";
	}
	
	return "";
};

auto_update();
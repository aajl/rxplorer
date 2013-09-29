function auto_update() {
	var downloaded = Number(sys.call('update.downloaded', 'explorer'));
	if(downloaded) {
		print("........................................\n");
		//sys.shell_execute(sys.path.app + "/explorer+.exe",  "-app -mutex auto_update_mutex -dui Modules/MDirectUI.dll -Module Modules/MUpdate.dll unused -skin update/skins/skin.xml -appname explorer");

		//return "exit";
	}
	
	return "";
};

auto_update();
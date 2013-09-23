function auto_update() {
	if(sys.updater.downloaded())
	{
		sys.shell_execute(sys.path.app + "/Explorer+.exe",  "-app -mutex explorer_plus_update_mutex -Module Modules/MUpdate.dll");
		return "exit";
	}
	
	return "";
}

auto_update();
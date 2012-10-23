var folder_index = 0;
var xplorer_left_pos = 0;
var session = [];
var session_open = {};
var curr_tab = null;
var favorite_folders = [];
var clicked_tab = false;

$(function(){
	print("load succeeded\n");
	
	sys.explorer.handler({
		open:function(path, display_name, view_id) {
			addr.path = path;
			curr_tab.text = display_name;
			curr_tab.check(true);
			curr_tab.vpath = path;
			
			//print("tab: " + curr_tab + " type: " + typeof(curr_tab) + "\n");
			print("open: " + path + " " + display_name + " " + view_id + "\n");
		},
		active:function(path, display_name, view_id) {
			addr.path = path;
			curr_tab.text = display_name;
			curr_tab.vpath = path;
			
			if(clicked_tab) {
				clicked_tab = false;
				filefilter.clear();
				sys.explorer.get_filter();
			}
			
			//print("tab: " + curr_tab.id + " type: " + typeof(curr_tab) + "\n");
			print("active: " + path + " " + display_name + " " + view_id + "\n");
		},
		selected:function(files) {
		if(files.length == 0) {
				statusbar.filename.text = curr_tab.text;
			} else if(files.length == 1) {
				var pth = new jpath(files[0]);
				var text = pth.filename();
				statusbar.filename.text = text.length == 0 ? files[0] : text;
			} else {
				statusbar.filename.text = "选择了 " + files.length + " 个文件";
			}
		},
		filter:function(types) {
			print("filter types: " + types.length + "\n");
			if(types.length == 0) {
				filefilter.clear();
			} else {
				for(var i = 0; i < types.length; ++i) {
					if(types[i] == "#1folder")
						filefilter.insert({"text":"显示文件夹", "icon":"icons.folder"});
					else if(types[i] == "#2hidden")
						filefilter.insert({"text":"显示隐藏文件", "icon":"icons.file"});
					else if(types[i] == "#3file")
						filefilter.insert({"text":"显示所有文件", "icon":"icons.file"});
					else
						filefilter.insert({"text":types[i], "icon":types[i]});
				}
			}
		},
	});

	var obj = sys.explorer.drives();
	
	// 计算drivebar实际所需宽度
	var width = 7 + obj.drives.length * 29 + obj.drives.length + 5;
	
	// 移动工具栏
	var offset = drivebar.width - width;
	drivebar.move(drivebar.x, drivebar.y, drivebar.width - offset, drivebar.height);
	addrbar.move(addrbar.x - offset, addrbar.y, addrbar.width + offset, addrbar.height);
	
	for(var i = 0; i < obj.drives.length; ++i) {
		var drv = obj.drives[i];		
		var percent = parseInt((drv.total - drv.free) / drv.total * 10);
		
		var item = {};
		item.id = "drive" + i;
		item.text = drv.drive;
		item.path = drv.drive + ":\\";
		item.pos = percent;
		item.icon = drv.drive + ":\\";
		item.down = (percent >= 9 ? "progress_hover_red.png" : "progress_hover.png");
		drivebar.insert(item);
	}
	
	load_session();
});

function on_close() {
	var children = xplorer.tabs.children;
	print("close " + children + " tabs \n");
	
	session = [];
	for(var i = 0; i < folder_index; ++i) {
		var tab = eval("xplorer.tabs.tab" + (i + 1));
		if(typeof(tab) == "undefined")
			continue;
		
		session.push({"name":tab.text, "path":tab.vpath});
		print("name: " + tab.text + " path: " + tab.vpath + "\n");
	}
	
	save2file(session, "session.json");
}

function save2file(data, path) {
	var file = new jfile;
    var app_path = sys.get_path(sys.app_path);
    if (!file.open(app_path + "\\" + path, "w"))
		return;

	file.write(data.toJSON());
	file.close();
	file = null;
}

function move_menu(menu_btn, menu_pane) {
    if (menu_btn.popuped)
        return;

    var rc_menu_btn = menu_btn.screen_rect();
    var rc_main_menu = menu_pane.rect();
    var top = rc_menu_btn.top + rc_menu_btn.height - 1;
    menu_pane.move(rc_menu_btn.left, top, rc_main_menu.width, rc_main_menu.height);
}

function move_main_menu() {
    move_menu(menu_file_btn, menu_file);
    move_menu(menu_bookmark_btn, menu_bookmark);
    move_menu(menu_plugin_btn, menu_plugin);
    move_menu(menu_tools_btn, menu_tools);
    move_menu(menu_windows_btn, menu_windows);
    move_menu(menu_help_btn, menu_help);
}

function load_favorite_tools() {
    var file = new jfile;
    var app_path = sys.get_path(sys.app_path);
    if (!file.open(app_path + "\\bookmark.json", "r"))
		return;

	var data = file.read();		
	if(data.length == 0)
		return;

	var bookmark = eval("(" + data + ")");
	var sys_path = sys.get_path(sys.sys_path);
	for (var i = 0; i < bookmark.length; ++i) {
		var path = bookmark[i].path;

		path = path.replace(/%Sys/gi, sys_path);
		path = path.replace(/%App/gi, app_path);

		var id = sys.hash(path);
		favtools.insert({"id":"btn" + id, "path":path, "icon":path + "|0|large"});
	}

    file = null;
}

function load_favorite_folders() {
    var file = new jfile;
    var app_path = sys.get_path(sys.app_path);
    if (!file.open(app_path + "\\folders.json", "r"))
		return;

	var data = file.read();
	if(data.length == 0)
		return;
		
	favorite_folders = eval("(" + data + ")");
	var sys_path = sys.get_path(sys.sys_path);
	for (var i = 0; i < favorite_folders.length; ++i) {
		var path = favorite_folders[i].path;
		var name = favorite_folders[i].name;
		
		path = path.replace(/%Sys/gi, sys_path);
		path = path.replace(/%App/gi, app_path);

		var id = sys.hash(path);
		favfolders.insert({"id":"folder" + id, "name":name, "path":path, "icon":path, "action":"open_folder(this.path)"});
	}

    file = null;
}

function load_session() {
	var file = new jfile;
    var app_path = sys.get_path(sys.app_path);
    if (!file.open(app_path + "\\session.json", "r"))
		return;

	var data = file.read();
	session = eval("(" + data + ")");
	for(var i = 0; i < session.length; ++i) {
		session_open[session[i].view] = 0;
	}
	
	if(session.length > 0) {
		new_tab_view(session[0].path, session[0].name);
		for(var i = 1; i < session.length; ++i) {
			new_tab(session[i].path, session[i].name);
		}
	} else {
		new_tab_view();
	}
		
	file = null;
}

function show_treeview() {
	if(treeview.visible()) {
		treeview.hide();
		xplorer.move(treeview.x, xplorer.y, xplorer.width + xplorer.x - treeview.x, xplorer.height);
		xplorer.redraw(true);
	} else {
		xplorer.move(247, xplorer.y, 630, xplorer.height);
		treeview.show();
		//treeview.redraw(true);
		xplorer.redraw(true);
		explorer.redraw(true);
	}
}

function new_tab(path, name) {
	++folder_index;
	
	// 增加一个tab按钮
	var tab_id = "tab" + folder_index;
	var view_id = "explorer.xplorer.views.view" + folder_index;
	xplorer.tabs.insert({"id":"." + tab_id, "tab":view_id, "text":name, "icon":path});
	
	var tab = eval("xplorer.tabs." + tab_id);
	tab.vpath = path;
	tab.index = folder_index;
	tab.uninit = true;
}

function new_tab_view(path, name) {
	++folder_index;

	if(typeof(path) == "undefined")
		path = "C:\\Windows\\System32";
		//path = "C:\\Windows\\";
	
	// 增加一个视图
	var view_id = "view" + folder_index;
	xplorer.views.insert({"id":"." + view_id});
	view_id = "xplorer.views." + view_id;
    var view = eval(view_id);
	
	// 增加一个tab按钮
	var tab_id = "tab" + folder_index;
	xplorer.tabs.insert({"id":"." + tab_id, "tab":view.id, "text":name, "icon":path});
	if(curr_tab != null)
		curr_tab.check(false);
	
	curr_tab = eval("xplorer.tabs." + tab_id);
	curr_tab.vpath = path;
	if(folder_index == 1)
		xplorer.tabs.tab1.check(true);
	
	var hid = sys.explorer.new(view.handler(), path, view.rect(), view_id);
}

function open_folder(path) {
	addr.path = path;
	sys.explorer.open(0, path);
}

function click_tab(tab) {
	curr_tab = tab;
	print("text " + tab.text + " " + tab.id + "\n");
	if(tab.uninit)
	{
		tab.uninit = false;
		
		// 增加一个视图
		var view_id = "view" + tab.index;
		xplorer.views.insert({"id":"." + view_id});
		view_id = "xplorer.views." + view_id;
		var view = eval(view_id);
		var hid = sys.explorer.new(view.handler(), tab.vpath, view.rect(), view_id);
	} else {
		clicked_tab = true;
	}
}

function favorite_folder(path) {
	for(var i = 0; i < favorite_folders.length; ++i) {
		if(path == favorite_folders[i].path)
			return;
	}
	
	var jpth = new jpath();
	jpth.open(path);
	var name = jpth.filename();
	if(name.length == 0)
		name = jpth.root_path();
	if(name.length == 0)
		return;
		
	var item = {};
	item.name = name;
	item.path = path;
	favorite_folders.push(item);

	item = {};
	item.name = name;
	item.path = path;
	var id = sys.hash(path);
	item.id = "folder" + id;
	item.icon = path;
	item.action = "open_folder(this.path)";
	favfolders.insert(item);
	
	save2file(favorite_folders, "folders.json");
}
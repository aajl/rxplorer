var folder_index = 0;
var xplorer_left_pos = 0;
var bookmark = [];
var favorite_folders = [];
var curr_tab = null;
var clicked_tab = false;
var poped_menu_tab = null;
var poped_menu_tool = null;
var poped_menu_xplor_id = null;
var setting = {};

$(function(){
	print("load succeeded\n");
		
	sys.explorer.handler({
		open:function(path, display_name, view_id) {
			curr_tab.text = display_name;
			curr_tab.check(true);
			curr_tab.path = path;
			curr_tab.set_icon(path);
			if(path.substr(0, 2) == "::" || path == "desktop")
				addr.path = display_name;
			else
				addr.path = path;
			
			//print("tab: " + curr_tab + " type: " + typeof(curr_tab) + "\n");
			print("open: " + path + " " + display_name + " " + view_id + "\n");
		},
		active:function(path, display_name, view_id) {
			curr_tab.text = display_name;
			curr_tab.path = path;
			if(setting.treeview.sync)
				sys.explorer.treeview_sync(curr_tab.path);
				
			if(path.substr(0, 2) == "::" || path == "desktop")
				addr.path = display_name;
			else
				addr.path = path;
			
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
				filefilter.set_redraw(false);
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
				filefilter.set_redraw(true);
				filefilter.redraw();
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
	
	drivebar.set_redraw(false);
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
	drivebar.set_redraw(true);
	drivebar.redraw();

	sys.explorer.treeview_new("treeview.tree.view", treeview.tree.view.rect());
	
	load_setting();
	
	load_session("xplorer", "session.json");
	load_session("xplorer2", "session2.json");
	
	if(!setting.treeview.show)
		show_treeview();
});

function on_close() {
	save_session(xplorer, "session.json");
	save_session(xplorer2, "session2.json");

	save2file(setting, "setting.json");
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

	bookmark = eval("(" + data + ")");
	var sys_path = sys.get_path(sys.sys_path);
	for (var i = 0; i < bookmark.length; ++i) {
		var path = bookmark[i].path;

		path = path.replace(/%Sys/gi, sys_path);
		path = path.replace(/%App/gi, app_path);

		var id = sys.hash(path);
		favtools.insert({"id":"btn" + id, "path":path, "icon":path + "|0|24"});
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

function save_session(xplor, filename) {
	var sess = [];
	var children = xplor.tabs.children;
	for(var i = 0; i < children; ++i) {
		var tab = xplor.tabs.child(i);
		sess.push({"name":tab.text, "path":tab.path});
	}
	
	save2file(sess, filename);
}

function load_session(xplor_id, filename, sess_open) {
	var file = new jfile;
    var app_path = sys.get_path(sys.app_path);
    if (!file.open(app_path + "\\" + filename, "r"))
		return;

	var data = file.read();
	if(data == null || data.length == 0)
		return;
	
	var sess = eval("(" + data + ")");
	file.close();
	file = null;
	
	if(sess.length > 0) {
		new_tab_view(xplor_id, sess[0].path, sess[0].name);
		for(var i = 1; i < sess.length; ++i) {
			new_tab(xplor_id, sess[i].path, sess[i].name);
		}
	} else {
		print("load session " + xplor_id + "\n");
		new_tab_view(xplor_id);
	}
}

function load_setting() {
	var file = new jfile;
    var app_path = sys.get_path(sys.app_path);
    if (!file.open(app_path + "\\setting.json", "r"))
		return;

	var data = file.read();
	if(data != null && data.length > 0) {
		setting = eval("(" + data + ")");
		file.close();
		file = null;
	}
	
	if(typeof(setting.treeview) == "undefined") {
		setting.treeview = {};
		setting.treeview.sync = false;
		setting.treeview.show = true;
	}
	
	if(setting.treeview.sync)
		sync.check(true);
}

function show_treeview() {
	if(treeview.visible()) {
		treeview.hide();
		xplorer.move(treeview.x, xplorer.y, xplorer.width + xplorer.x - treeview.x, xplorer.height);
		xplorer.redraw(true);
		setting.treeview.show = false;
	} else {
		xplorer.move(247, xplorer.y, 630, xplorer.height);
		treeview.show();
		//treeview.redraw(true);
		xplorer.redraw(true);
		explorer.redraw(true);
		setting.treeview.show = true;
	}
}

function new_tab(xplor_id, path, name) {
	++folder_index;
	
	// 增加一个tab按钮
	var tab_id = "tab" + folder_index;
	var view_id = "explorer." + xplor_id + ".views.view" + folder_index;
	var view_obj = xplor_id + ".views.view" + folder_index;

	var xplor = eval(xplor_id);
	xplor.tabs.insert({"id":"." + tab_id, "tab":view_id, "view": view_obj, "text":name, "icon":path});
	
	var tab = eval(xplor_id + ".tabs." + tab_id);
	tab.path = path;
	tab.index = folder_index;
	tab.uninit = true;
}

function new_tab_view(xplor_id, path, name) {
	++folder_index;

	if(typeof(path) == "undefined")
		path = "Computer";
	
	var xplor = eval(xplor_id);
	
	// 增加一个视图
	var view_id = "view" + folder_index;
	xplor.views.insert({"id":"." + view_id});
	view_id = xplor_id + ".views." + view_id;
    var view = eval(view_id);
	
	// 增加一个tab按钮
	var tab_id = "tab" + folder_index;
	xplor.tabs.insert({"id":"." + tab_id, "tab":view.id, "view": view_id, "text":name, "icon":path});
	if(curr_tab != null)
		curr_tab.check(false);
	
	curr_tab = eval(xplor_id + ".tabs." + tab_id);
	curr_tab.path = path;
	if(folder_index == 1)
		xplor.tabs.tab1.check(true);
	
	var hid = sys.explorer.new(view.handler(), path, view.rect(), view_id);
}

function open_folder(path) {
	addr.path = path;
	sys.explorer.open(0, path);
}

function click_tab(xplor_id, tab) {
	curr_tab = tab;
	print(tab.id + "\n");
	if(tab.uninit)
	{
		tab.uninit = false;
		
		var xplor = eval(xplor_id);
		
		// 增加一个视图
		var view_id = "view" + tab.index;
		xplor.views.insert({"id":"." + view_id});
		view_id = xplor_id + ".views." + view_id;
		var view = eval(view_id);
		var hid = sys.explorer.new(view.handler(), tab.path, view.rect(), view_id);
	} else {
		clicked_tab = true;
	}
}

function close_tab(xplor, tab) {
	sys.explorer.remove(tab.view);
	xplor.tabs.remove(tab.id);
	xplor.views.remove(tab.tab);
}

function up() {
	sys.explorer.up();
}

function pop_tab_menu(xplor_id, tab, id) {
	print(tab + " " + id + " " + tab.tab + " " + tab.view + "\n");
	poped_menu_tab = tab;
	poped_menu_xplor_id = xplor_id;
	window.popmenu(tabmenu_pane.id);
}

function duplicate_tab(xplor_id) {
	new_tab_view(xplor_id, poped_menu_tab.path);
}

function add_favorite_folder(path) {
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

function pop_tool_menu(tool) {
	poped_menu_tool = tool;
	window.popmenu(toolmenu_pane.id);
}

function remove_favorite_tool(tool) {
	for(var i = 0; i < bookmark.length; ++i) {
		if(bookmark[i].path == tool.path) {
			bookmark.splice(i, 1);
			save2file(bookmark, "bookmark.json");
			break;
		}
   } 

   favtools.remove(tool.id);
}

function add_favorite_tools(files) {
	if(files.length == 0)
		return;
		
	for(var i = 0; i < files.length; ++i) {
		var path = files[i];
		var jpth = new jpath();
		jpth.open(path);
		var name = jpth.filename();
		var id = sys.hash(path);
		favtools.insert({"id":"btn" + id, "path":path, "icon":path + "|0|24"});
		
		var item = {};
		item.name = name;
		item.path = path;
		bookmark.push(item);
	}
	
	save2file(bookmark, "bookmark.json");
}

function on_dropfiles(files) {
	if(files.length == 0)
		return;
		
	var rc = favtools.screen_rect();
	var pt = sys.cursor_pos();
	if(rc.pt_in_rect(pt.x, pt.y)) {
		add_favorite_tools(files);
	}
}

function sync_treeview() {
	setting.treeview.sync = !setting.treeview.sync;
	if(setting.treeview.sync) {
		sys.explorer.treeview_sync(curr_tab.path);
	}
}

function locate_tool(path) {
	var pth = new jpath(path);
	sys.shell_execute(pth.remove_filename());
}
var folder_index = 0;
var xplorer_left_pos = 0;
var setting = {};
var editor = [];
var viewer = [];
var bookmark = [];
var selected_files = [];
var favorite_folders = [];
var curr_tab = null;
var pane1_curr_tab = null
var pane2_curr_tab = null;
var clicked_tab = false;
var poped_menu_tab = null;
var poped_menu_tool = null;
var poped_menu_bkmrk = null;
var poped_menu_xplor_id = null;
var session_open = false;
var session2_open = false;
var maximize = false;
var new_version = "";
var opened = false;
var extension = new jpath();

function get_drive_icon(type) {
	if(type == "fixed")
		return "icons.fixed";
		
	if(type == "cdrom")
		return "icons.cdrom";
		
	return "icons.removable";
}

function insert_drives(drives, preload) {
	drivebar.set_redraw(false);
	for(var i = 0; i < drives.length; ++i) {
		var drv = drives[i];
		var percent = drv.total == 0 ? 0 : parseInt((drv.total - drv.free) / drv.total * 10);
		
		var item = {};
		item.id = "drive" + i;
		item.drive = drv.drive;
		item.text = drv.drive;
		item.path = drv.drive + ":\\";
		item.pos = percent;
		item.icon = (preload && drv.total == 0) ? get_drive_icon(drv.type) : (drv.drive + ":\\");
		item.down = (percent >= 9 ? "progress_hover_red.png" : "progress_hover.png");
		item.label = drv.label;
		item.free = drv.free;
		item.total = drv.total;
		drivebar.insert(item);
	}
	drivebar.set_redraw(true);
	drivebar.redraw();
}

function format_disk_size(size) {
	var filesize = "";
	if(size < 1024)
		filesize += size + " " + lang.get("byte");
	else if(size < 1024 * 1024)
		filesize += (size / 1024).toFixed(2) + " KB";
	else if(size < 1024 * 1024 * 1024)
		filesize += (size / 1024 / 1024).toFixed(2) + " MB";
	else
		filesize += (size / 1024 / 1024 / 1024).toFixed(2) + " GB";
	
	return filesize;
}

function on_close() {
	save_session(xplorer, "session.json");
	save_session(xplorer2, "session2.json");

	save2file(setting, "setting.json");
}

function json_from_file(path, def) {
	var file = new jfile;
    if (!file.open(sys.path.app + "\\" + path, "r"))
		return def;

	var data = file.read();
	file.close();
	if(data.length == 0)
		return def;

	return eval("(" + data + ")");
}

function save2file(data, path) {
	var file = new jfile;
    if (!file.open(sys.path.app + "\\" + path, "w"))
		return;

	file.write(data.toJSON());
	file.close();
	file = null;
}

function resize_favorite_folder_panel() {
	if(favorite_folders.length > 0) {
		var rc = favfolders_pane.rect();
		var height = 23 + 1 + 3 + 1 + favorite_folders.length * 23 + favorite_folders.length - 1 + 4;
		favfolders_pane.move(rc.x, rc.y, rc.width, height);
	}
}

function load_favorite_tools() {
	bookmark = json_from_file("bookmark.json", []);
	
	for (var i = 0; i < bookmark.length; ++i) {
		var bkmrk = bookmark[i];
		var path = bkmrk.path;

		path = path.replace(/%Sys/gi, sys.path.system);
		path = path.replace(/%App/gi, sys.path.app);

		var accl = "";
		if(typeof(bkmrk.key) != "undefined" && bkmrk.key.length > 0) {
			if(typeof(bkmrk.vkey) != "undefined" && bkmrk.vkey.length > 0)
				accl = bkmrk.vkey + "+" + bkmrk.key;
			else
				accl = bkmrk.key;
		}
		
		var id = sys.hash(bkmrk.name + path + param);
		var param = (typeof(bkmrk.param) == "undefined") ? "" : bkmrk.param;
		favtools.insert({"id":"btn" + id, "path":path, "icon":path + "|0|24", "param": param, "tooltip": bkmrk.name, "accel": accl});
			
		if(typeof(bkmrk.key) != "undefined" && bkmrk.key.length > 0) {
			path = bkmrk.path.replace(/\\/gi, "\\\\").replace(/"/gi, "\\\"");
			param = bkmrk.param.replace(/\\/gi, "\\\\").replace(/"/gi, "\\\"");
			accel.add(bkmrk.vkey, bkmrk.key, "open_tool(\"" + path + "\", \"" + param + "\")");
		}
	}
}

function load_favorite_folders() {
	favorite_folders = json_from_file("folders.json", []);
	
	for (var i = 0; i < favorite_folders.length; ++i) {
		var path = favorite_folders[i].path;
		var name = favorite_folders[i].name;
		
		path = path.replace(/%Sys/gi, sys.path.system);
		path = path.replace(/%App/gi, sys.path.app);

		var id = sys.hash(path);
		favfolders.insert({"id":"folder" + id, "name":name, "path":path, "icon":path, "action":"open_folder(this.path)"});
	}
	
	resize_favorite_folder_panel();
}

function save_session(xplor, filename) {
	var sess = [];
	var children = xplor.tabs.children;
	for(var i = 0; i < children; ++i) {
		var tab = xplor.tabs.child(i);
		sess.push({"name": tab.text, "path": tab.path, "mode": tab.mode});
	}
	
	save2file(sess, filename);
}

function load_session(xplor_id, filename, sess_open) {
	var sess = json_from_file(filename, []);	
	if(sess.length > 0) {
		new_tab_view(xplor_id, sess[0].path, sess[0].name, sess[0].mode);
		for(var i = 1; i < sess.length; ++i) {
			new_tab(xplor_id, sess[i].path, sess[i].name, sess[i].mode);
		}
	} else {
		new_tab_view(xplor_id);
	}
}

function load_setting() {
	setting = json_from_file("setting.json", {});
	
	if(typeof(setting.treeview) == "undefined") {
		setting.treeview = {};
		setting.treeview.sync = false;
		setting.treeview.show = true;
	}
	
	if(typeof(setting.explorer) == "undefined") {
		setting.explorer = {};
		setting.explorer.max = false;
		setting.explorer.dual = true;
	} else {
		if(typeof(setting.explorer.lang) != "undefined")
			change_language(setting.explorer.lang);
	}
	
	if(typeof(setting.statusbar) == "undefined") {
		setting.statusbar = {};
		setting.statusbar.show = true;
	}
	
	if(setting.treeview.sync)
		sync.check(true);
}

function show_treeview(show) {
	var rc = xplr.rect();
	var width = treeview.width + 2;
	if(show) {
		xplr.move(245, rc.y, rc.width - width, rc.height);
		xplrfrm.move(245, xplrfrm.y, xplrfrm.width - width, xplrfrm.height);
		treeview.show();
		treeview.redraw(true);
		xplr.redraw(true);
		setting.treeview.show = true;
	} else {
		treeview.hide();
		xplr.move(3, rc.y, rc.width + width, rc.height);
		xplrfrm.move(3, xplrfrm.y, xplrfrm.width + width, xplrfrm.height);
		xplr.redraw(true);
		setting.treeview.show = false;
	}

	explorer.redraw(true);
}

function new_tab(xplor_id, path, name, mode) {
	++folder_index;
	
	// 增加一个tab按钮
	var tab_id = "tab" + folder_index;
	var tab_obj = xplor_id + ".tabs." + tab_id;
	var view_id = "explorer.xplr." + xplor_id + ".views.view" + folder_index;
	var view_obj = xplor_id + ".views.view" + folder_index;

	var xplor = eval(xplor_id);
	xplor.tabs.insert({"id":"." + tab_id, "tab":view_id, "view": view_obj, "text":name, "icon":path});
	
	var tab = eval(tab_obj);
	tab.path = path;
	tab.mode = mode;
	tab.obj = tab_obj;
	tab.index = folder_index;
	tab.uninit = true;
}

function new_tab_view(xplor_id, path, name, mode) {
	++folder_index;

	print("new tab view: " + xplor_id + " " + path + "\n");
	
	if(typeof(path) == "undefined")
		path = "Computer";
	
	var xplor = eval(xplor_id);
	
	var view_id = "view" + folder_index;
	var view_obj = xplor_id + ".views." + view_id;
	
	var tab_id = "tab" + folder_index;
	var tab_obj = xplor_id + ".tabs." + tab_id;
	
	// 增加一个视图
	xplor.views.insert({"id":"." + view_id, "tabobj": tab_obj});
    var view = eval(view_obj);
	view.tabobj = tab_obj;

	// 增加一个tab按钮
	xplor.tabs.insert({"id":"." + tab_id, "tab":view.id, "view": view_obj, "text":name, "icon":path});	
	curr_tab = eval(tab_obj);
	curr_tab.path = path;
	curr_tab.mode = mode;
	curr_tab.obj = tab_obj;
	curr_tab.index = folder_index;
	curr_tab.click();
	
	var hid = sys.explorer.new(view.handler(), path, view.rect(), view_obj, mode);
}

function open_folder(path) {
	addr.path = path;
	sys.explorer.open(0, path);
}

function click_tab(xplor_id, tab) {
	curr_tab = tab;
	print(tab.id + "\n");

	if(tab.uninit) {
		tab.uninit = false;
		
		var xplor = eval(xplor_id);
		
		// 增加一个视图
		var view_id = "view" + tab.index;
		var view_obj = xplor_id + ".views." + view_id;
		xplor.views.insert({"id":"." + view_id});
		var view = eval(view_obj);
		view.tabobj = tab.obj;
		var hid = sys.explorer.new(view.handler(), tab.path, view.rect(), view_obj);
	} else {
		clicked_tab = true;
	}
}

function close_tab(xplor, tab) {
	if(xplor.tabs.children == 0)
		return;
	
	var index = xplor.tabs.get_index(tab.id);
	sys.explorer.remove(tab.view);
	xplor.tabs.remove(tab.id);
	xplor.views.remove(tab.tab);
	if(xplor.tabs.children == 0)
		return;

	if(--index <= 0)
		index = 0;
		
	var btn = xplor.tabs.child(index);
	btn.click();
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
	
	resize_favorite_folder_panel();
}

function remove_favorite_folder(path, folder_id) {
	for(var i = 0; i < favorite_folders.length; ++i) {
		if(path == favorite_folders[i].path) {
			favorite_folders.splice(i, 1);
			save2file(favorite_folders, "folders.json");
			resize_favorite_folder_panel();
			
			delete_ctrl_item(favfolders, folder_id);
			break;
		}
	}
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

function open_tool(path, param) {
	if(param.length > 0) {
		if(selected_files.length > 0) {
			var file = param;
			param = "";
			for(var i = 0; i < selected_files.length; ++i) {
				var fl = file;
				var jpth = new jpath(selected_files[i]);
				fl = fl.replace(/%FullPath/gi, selected_files[i]);
				fl = fl.replace(/%Path/gi, jpth.remove_filename());
				
				param = param + fl + " ";
			}
		} else {
			param = param.replace(/%Path/gi, addr.path).replace(/%FullPath/gi, "");
		}
	}
	
	param = param.replace(/'/gi, "\"");
	if(param == "\"\"")
		param = "";	
	
	print(path + " " + param + "\n");
	
	sys.shell_execute(path, param);
}

function tool_setting(tool) {
	toolsetting.tooltip.text = tool.tooltip;
	toolsetting.accel.text = tool.accel;
	toolsetting.cmd.text = tool.path;
	toolsetting.param.text = tool.param;
	
	poped_menu_bkmrk = null;
	for (var i = 0; i < bookmark.length; ++i) {
		var bkmrk = bookmark[i];

		var path = bkmrk.path;
		path = path.replace(/%Sys/gi, sys.path.system);
		path = path.replace(/%App/gi, sys.path.app);

		if(path == tool.path) {
			if(typeof(bkmrk.param) == "undefined")
				poped_menu_bkmrk = bkmrk;
			else if(bkmrk.param == tool.param)
				poped_menu_bkmrk = bkmrk;
			else
				continue;
			
			break;
		}
	}
	
	toolsetting.show();
}

function set_tool_info() {
	if(poped_menu_bkmrk == null || poped_menu_tool == null)
		return;
	
	poped_menu_tool.tooltip = toolsetting.tooltip.text;
	poped_menu_tool.accel = toolsetting.accel.text;
	poped_menu_tool.path = toolsetting.cmd.text;
	poped_menu_tool.param = toolsetting.param.text;
	
	poped_menu_bkmrk.name = toolsetting.tooltip.text;
	poped_menu_bkmrk.path = toolsetting.cmd.text;
	poped_menu_bkmrk.param = toolsetting.param.text;
	
	save2file(bookmark, "bookmark.json");
}

function copy_file() {
	if(selected_files.length == 0 || pane1_curr_tab == null || pane2_curr_tab == null)
		return;
	
	if(!pane1_curr_tab.visible() || !pane2_curr_tab.visible())
		return;
	
	if(pane1_curr_tab == curr_tab) {
		print("select files: " + selected_files[0] + " other: " + pane2_curr_tab.path + "\n");
		sys.explorer.copyfile(selected_files, pane2_curr_tab.path);
	} else if(pane2_curr_tab == curr_tab) {
		print("select files: " + selected_files[0] + " other: " + pane1_curr_tab.path + "\n");
		sys.explorer.copyfile(selected_files, pane1_curr_tab.path);
	}
}

function move_file() {
	if(selected_files.length == 0 || pane1_curr_tab == null || pane2_curr_tab == null)
		return;
	
	if(!pane1_curr_tab.visible() || !pane2_curr_tab.visible())
		return;
	
	if(pane1_curr_tab == curr_tab) {
		print("select files: " + selected_files[0] + " other: " + pane2_curr_tab.path + "\n");
		sys.explorer.movefile(selected_files, pane2_curr_tab.path);
	} else if(pane2_curr_tab == curr_tab) {
		print("select files: " + selected_files[0] + " other: " + pane1_curr_tab.path + "\n");
		sys.explorer.movefile(selected_files, pane1_curr_tab.path);
	}
}

function new_folder() {
	sys.explorer.newfolder("");
}

function on_accel(accel) {
	print(accel + "\n");
}

function find_editor(ext) {
	print("ext " + ext + "\n");
	for(var i = 0; i < editor.length; ++i) {
		if(editor[i].ext.toLowerCase() == ext)
			return editor[i].editor;
	}
	
	return editor.length == 0 ? null : editor[0].editor;
}

function edit_file() {
	for(var i = 0; i < selected_files.length; ++i) {
		var path = new jpath(selected_files[i]);
		var edtr = find_editor(path.ext().toLowerCase());
		if(edtr == null)
			continue;
		
		sys.shell_execute(edtr, "\"" + selected_files[i] + "\"");
	}
}

function find_viewer(ext) {
	print("ext " + ext + "\n");
	for(var i = 0; i < viewer.length; ++i) {
		if(viewer[i].ext.toLowerCase() == ext)
			return viewer[i].viewer;
	}
	
	return viewer.length == 0 ? null : viewer[0].viewer;
}

function view_file() {
	for(var i = 0; i < selected_files.length; ++i) {
		var path = new jpath(selected_files[i]);
		var edtr = find_viewer(path.ext().toLowerCase());
		if(edtr == null)
			continue;
		
		sys.shell_execute(edtr, "\"" + selected_files[i] + "\"");
	}
}

function open_curr_folder() {
	if(curr_tab == null)
		return;
	
	sys.shell_execute(curr_tab.path);
}

function filter_files() {
	var children = filefilter.children;
	if(children == 0)
		return;
	
	var filters = {};
	for(var i = 0; i < children; ++i) {
		var flt = filefilter.child(i);
		filters[flt.ext] = flt.checked;
	}
	
	sys.explorer.set_filters(filters);
}

function clear_filters() {
	sys.explorer.set_filters();
	sys.explorer.set_search_filter("");
	fastfilter.text = "";
	sys.explorer.get_file_types();
}

function show_statusbar(show, init) {
	setting.statusbar.show = show;
	var height = detailbar.height - statusbar.height;
	if(show) {
		detailbar.hide();
		statusbar.show();
		statusbar.redraw();
		
		var rc = treeview.rect();
		treeview.move(rc.x, rc.y, rc.width, rc.height + height);
		treeview.redraw(true);
		
		rc = xplr.rect();
		xplr.move(rc.x, rc.y, rc.width, rc.height + height);
		xplr.redraw(true);
	} else {
		statusbar.hide();
		detailbar.show();
		detailbar.redraw();
		
		var rc = treeview.rect();
		treeview.move(rc.x, rc.y, rc.width, rc.height - height);
		treeview.redraw(true);
		
		rc = xplr.rect();
		xplr.move(rc.x, rc.y, rc.width, rc.height - height);
		xplr.redraw(true);
	}
}

function get_curr_xplorer() {
	if(typeof(curr_tab) == "undefined" || curr_tab == null)
		return "xplorer";
	
	if(pane2_curr_tab == curr_tab)
		return "xplorer2";
	
	return "xplorer";
}

function copy_curr_path() {
	sys.clipboard(addr.path);
}

function copy_file_path() {
	if(selected_files.length > 0)
		sys.clipboard(selected_files[0]);
}

function show_tooltip(ctrl, info, accl) {
	var text = info;
	if(typeof(accl) != "undefined" && accl.length > 0)
		text += "    " + accl;
	
	var rc = ctrl.screen_rect();
	var size = tooltip.tip.get_text_size(text);
	var width = size.width + 8;
	tooltip.move(rc.x, rc.y + rc.height + 1, size.width + 6, tooltip.height);
	
	tooltip.tip.text = text;
	tooltip.show();
}

function show_drive_tooltip(drive) {
	var text = "";
	if(drive.label.length > 0)
		text = drive.label + " (" + drive.drive + ":)";
	else
		text = drive.drive + ":";
	
	var rc = drive.screen_rect();
	var size = drivetip.tip.get_text_size(text);
	drivetip.move(rc.x, rc.y + rc.height + 1, drivetip.width, drivetip.height);

	var free = Number(drive.free) * 1024 * 1024;
	var total = Number(drive.total) * 1024 * 1024;
	drivetip.tip.text = text;
	drivetip.spaceinfo.text = format_disk_size(free) + " 可用, 共" + format_disk_size(total);
	drivetip.icon.set_icon(drive.path + "|0|48");
	
	var percent = (total - free) / total * 100;
	if(percent > 90)
		drivetip.space.state = 1;
	else
		drivetip.space.state = 0;
	
	if(total == 0)
		drivetip.space.pos = 0;
	else
		drivetip.space.pos = percent;
	
	drivetip.show();
}

function show_folder_list(xplor, xplor_id) {
	var children = xplor.tabs.children;
	if(children == 0) {
		return;
	}
	
	folderlist.clear();
	for(var i = 0; i < children; ++i) {
		var tab = xplor.tabs.child(i);
		folderlist.insert({"name": tab.text, "icon": tab.path, "action": xplor_id + ".tabs.active('" + tab.id+ "')"});
	}
	
	var height = children * 23 + children - 1 + 4;
	folderlist_pane.move(folderlist_pane.x, folderlist_pane.y, folderlist_pane.width, height);
	folderlist_pane.redraw();
}

function show_dual_pane(show) {
	if(show) {
		var rc = xplorer2.rect();
		var width = (xplrfrm.width - 2) / 2;
		xplorer.move(xplorer.x, xplorer.y, width, xplorer.height);
		xplorer2.move(xplorer.x + width + 2, xplorer.y, width, xplorer.height);
		xplorer2.show();
		xplr.redraw();
		setting.explorer.dual = true;
		detailbar.tools.show();
		statusbar.tools.show();
	} else {
		xplorer2.hide();
		var rc = xplorer2.rect();
		xplorer.move(xplorer.x, xplorer.y, xplrfrm.width, xplorer.height);
		xplorer.redraw();
		setting.explorer.dual = false;
		detailbar.tools.hide();
		statusbar.tools.hide();
	}
}

function load_historys() {
	historys.clear();
	
	var curr = sys.explorer.curr_history();
	var hstrs = sys.explorer.historys();
	for(var i = hstrs.length - 1; i >= 0; --i) {
		var hstr = hstrs[i];
		var jpth = new jpath(hstr);
		historys.insert({"name": jpth.display_name(), "icon": hstr , "check": curr == i});
	}
	
	var height = hstrs.length * 23 + hstrs.length - 1 + 4;
	historys_pane.move(historys_pane.x, historys_pane.y, historys_pane.width, height);
}

function forward() {
	sys.explorer.forward();
}

function backward() {
	sys.explorer.backward();
}

function draw_gesture(cnvs, size, gesture) {
	var color = "#000000";
	if(gesture == "U") {
		cnvs.draw_line({"x":8, "y":13}, {"x":8, "y":2}, 2, color);
		cnvs.draw_line({"x":4, "y":6}, {"x":11, "y":6}, 1, color);
		cnvs.draw_line({"x":4, "y":7}, {"x":8, "y":3}, 2, color);
		cnvs.draw_line({"x":11, "y":7}, {"x":7, "y":3}, 2, color);
	}
}

function resize_panel(pnl, item_count, item_height, margin) {
	var height = 2 + item_height * item_count + margin * (item_count - 1) + 2;
	pnl.move(pnl.x, pnl.y, pnl.width, height);
}

function set_view_mode() {
	var modes = [7, 6, 1, 3, 4];
	if(sys.os.version >= 6.0)
		modes = [18, 17, 1, 2, 3, 4, 6];
		
	var mode = sys.explorer.view_mode();
	print(mode);
	
	var index = modes.length - 1;
	for(var i in modes) {
		if(modes[i] == mode) {
			index = Number(i);
			break;
		}
	}
	
	set_curr_view_mode(modes[(index + 1) % modes.length]);
}

function set_curr_view_mode(mode) {
	curr_tab.mode = mode;
	sys.explorer.view_mode(mode);
}

function show_search_wnd(text, path) {
	if(searcher.list.files.handler() == 0)
		searcher.list.files.attach(sys.search.create_result_wnd(searcher.handler(), searcher.list.files.rect()));
	
	searcher.show();
	if(text != null && text != "" && searchtxt.value.text != path + "\\" + text) {
		search();
		searchtxt.value.text = path + "\\" + text;
	}
}

function search() {
	var text = searchtxt.value.text;
	if(text == null || text.length <= 0)
		return;
	
	searcher.btn.enable(false);
	sys.search.search(text);
}

function on_scan_completed(exit_code) {
	if(exit_code == 1)
		sys.search.reload();
}

function change_language(language) {
	var langg = eval(language);
	lang.change(langg.id);
	
	setting.explorer.lang = language;
}

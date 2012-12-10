var folder_index = 0;
var xplorer_left_pos = 0;
var setting = {};
var editer = [];
var bookmark = [];
var selected_files = [];
var favorite_folders = [];
var curr_tab = null;
var pane1_curr_tab = null
var pane2_curr_tab = null;
var clicked_tab = false;
var poped_menu_tab = null;
var poped_menu_tool = null;
var poped_menu_xplor_id = null;
var session_open = false;
var session2_open = false;
var maximize = false;

// 1. 左边的不能新标签 √
// 11. 文件夹内双击空白处不能返回上一级 √
// 12. 状态栏高度应该可调整,可变为简洁模式和详细模式. √
// 4. 关闭标签后没有自动选择一个标签 √
// 5. 隐藏文件夹后坐标错误 √
// 2. 过滤器不能用 √
// 27. edit控件的内嵌edit坐标不对 √
// 26. img控件不支持图标 √
// 22. 无多语言 √
// 18. 弹出菜单在点击文件夹时不自动隐藏. √
// 7. 地址栏不能使用 √
// 17. 弹出窗口的高度不能根据项数自动计算. √
// 8. 状态栏信息显示不全 √
// 24 过滤器无滚动条
// 14. 改变大小后,第二面板的关闭按钮不见了.
// 20. 改变窗口宽度时,文件夹窗口大小发生异常,且默认状态下宽度也偏宽.
// 13. 常用工具栏多数工具不可用,并且太少.
// 25 过滤器的快速过滤不能用
// 3. 驱动器栏要用异步的方式
// 6. 第二面板不能隐藏
// 9. 菜单栏不能用
// 15. 点击标签右边的关闭不能关闭标签.
// 16. 标签过多时,无滚动条或其它方式显示所有标签.
// 19. 无tooltip
// 23. 无undo/redo
// 21. 无各类设置窗口.
// 10. 搜索栏不能用

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
			if(setting.treeview.sync)
				sys.explorer.treeview_sync(path);
			
			if(path.substr(0, 2) == "::" || path == "desktop")
				addr.path = display_name;
			else
				addr.path = path;
						
			if(clicked_tab) {
				curr_tab.text = display_name;
				curr_tab.path = path;
				clicked_tab = false;
				filefilter.clear();
			} else {
				var view = eval(view_id);
				curr_tab = eval(view.tabobj);
			}
			
			print("active: " +  " name: " + display_name + ", curr_view: " + view_id + ", curr pane: " + view_id.split(".", 1) + "\n");
			if(view_id.split(".", 1) == "xplorer") {
				pane1_curr_tab = curr_tab;
			}
			else if(view_id.split(".", 1) == "xplorer2") {
				pane2_curr_tab = curr_tab;
			}
			
			sys.explorer.get_file_types();
			
			session_open = true;
			if(!session2_open) {
				session2_open = true;
				load_session("xplorer2", "session2.json");
			}
		},
		selected:function(files) {
			selected_files = files;
			if(files.length == 0) {
				if(setting.statusbar.show) {
					statusbar.filename.text = curr_tab.text;
				} else {
					detailbar.info.clear();
					detailbar.info.insert({"text": curr_tab.text});
				}
			} else if(files.length == 1) {
				var pth = new jpath(files[0]);
				
				var filesize = "";
				if(pth.filesize() > 0) {
					filesize += lang.get("size") + ": ";
					var fsize = pth.filesize();
					if(fsize < 1024)
						filesize += fsize + " " + lang.get("byte");
					else if(fsize < 1024 * 1024)
						filesize += (fsize / 1024).toFixed(2) + " KB";
					else if(fsize < 1024 * 1024 * 1024)
						filesize += (fsize / 1024 / 1024).toFixed(2) + " MB";
					else
						filesize += (fsize / 1024 / 1024 / 1024).toFixed(2) + " GB";
				}
				
				if(setting.statusbar.show) { // 显示简单状态栏里的信息
					var fileinfo = pth.display_name();
					fileinfo += "    (" + pth.filetype() + ")";
					fileinfo += "    " + filesize;
					fileinfo += "    " + lang.get("create_time") + ": " + pth.modify_time();
					fileinfo += "    " + lang.get("modify_time") + ": " + pth.create_time();
					statusbar.filename.text = fileinfo;
				} else { // 显示详细状态栏里的信息
					detailbar.info.clear();
					detailbar.info.insert({"text": pth.display_name()});
					detailbar.info.insert({"text": filesize});
					detailbar.info.insert({"text": pth.filetype(), "color": "#5A6779"});
					detailbar.info.insert({"text": " "});
					detailbar.info.insert({"text": lang.get("create_time") + ": " + pth.modify_time()});
					detailbar.info.insert({"text": lang.get("modify_time") + ": " + pth.create_time()});
				}
			} else {
				if(setting.statusbar.show) {
					statusbar.filename.text = "选择了 " + files.length + " 个文件";
				} else {
					detailbar.info.clear();
					detailbar.info.insert({"text": "选择了 " + files.length + " 个文件"});
				}
			}
		},
		filetypes:function(types) {
			filefilter.clear();
			if(types.length > 0) {
				var filters = sys.explorer.get_filters();				
				filefilter.set_redraw(false);
				for(var i = 0; i < types.length; ++i) {
					var check = filters[types[i]];
					if(types[i] == "#1folder")
						filefilter.insert({"text":"显示文件夹", "icon":"icons.folder", "ext": types[i], "id": ".filterfolder", "check":typeof(check) == "undefined" ? true : check});
					else if(types[i] == "#2hidden")
						filefilter.insert({"text":"显示隐藏文件", "icon":"icons.hiddenfile", "ext": types[i], "id": ".filterhidden", "check":typeof(check) == "undefined" ? true : check});
					else if(types[i] == "#3file")
						filefilter.insert({"text":"显示所有文件", "icon":"icons.allfile", "ext": types[i], "id": ".filterfile", "check":typeof(check) == "undefined" ? true : check});
					else
						filefilter.insert({"text":types[i], "icon":types[i], "ext": types[i], "id": ".filter" + sys.hash(types[i]), "check":check});
				}
				filefilter.set_redraw(true);
				filefilter.redraw();
			}
		},
		drives:function(drives) {
		},
		dbclk:function() {
			up();
		}
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
	
	if(setting.statusbar.show)
		show_statusbar(true);

	if(!setting.treeview.show)
		show_treeview(false);
	
	if(setting.explorer.max)
		explorer.max();
	
	editer = json_from_file("editer.json", []);
	for(var i = 0; i < editer.length; ++i) {
		editer[i].editer = editer[i].editer.replace(/%Sys/gi, sys.path.system);
		editer[i].editer = editer[i].editer.replace(/%App/gi, sys.path.app);
	}
});

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
    move_menu(menu_edit_btn, menu_edit);
    move_menu(menu_bookmark_btn, menu_bookmark);
    move_menu(menu_plugin_btn, menu_plugin);
    move_menu(menu_tools_btn, menu_tools);
    move_menu(menu_help_btn, menu_help);
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

		var id = sys.hash(bkmrk.name + path + param);
		var param = (typeof(bkmrk.param) == "undefined") ? "" : bkmrk.param;
		favtools.insert({"id":"btn" + id, "path":path, "icon":path + "|0|24", "param": param, "tooltip": bkmrk.name});
			
		print("key: " + bkmrk.key + "\n");

		if(typeof(bkmrk.key) != "undefined" && bkmrk.key.length > 0) {
			path = bkmrk.path.replace(/\\/gi, "\\\\");
			path = path.replace(/"/gi, "\\\"");
			param = bkmrk.param.replace(/\\/gi, "\\\\");
			param = param.replace(/"/gi, "\\\"");
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
		sess.push({"name": tab.text, "path": tab.path});
	}
	
	save2file(sess, filename);
}

function load_session(xplor_id, filename, sess_open) {
	var sess = json_from_file(filename, []);	
	if(sess.length > 0) {
		new_tab_view(xplor_id, sess[0].path, sess[0].name);
		for(var i = 1; i < sess.length; ++i) {
			new_tab(xplor_id, sess[i].path, sess[i].name);
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
	}
	
	if(typeof(setting.statusbar) == "undefined") {
		setting.statusbar = {};
		setting.statusbar.show = false;
	}
	
	if(setting.treeview.sync)
		sync.check(true);
}

function show_treeview(show) {
	var rc = xplr.rect();
	var width = treeview.width + 2;
	if(show) {
		xplr.move(245, rc.y, rc.width - width, rc.height);
		treeview.show();
		treeview.redraw(true);
		xplr.redraw(true);
		setting.treeview.show = true;
	} else {
		treeview.hide();
		xplr.move(3, rc.y, rc.width + width, rc.height);
		xplr.redraw(true);
		setting.treeview.show = false;
	}

	explorer.redraw(true);
}

function new_tab(xplor_id, path, name) {
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
	tab.obj = tab_obj;
	tab.index = folder_index;
	tab.uninit = true;
}

function new_tab_view(xplor_id, path, name) {
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
	curr_tab.click();
	
	var hid = sys.explorer.new(view.handler(), path, view.rect(), view_obj);
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
			
			var ids = folder_id.split(".");
			ids.splice(ids.length - 1, 1);
			folder_id = ids.join(".");
			favfolders.remove(folder_id);
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
			param = param.replace(/%Path/gi, addr.path);
		}
	}
	
	param = param.replace(/'/gi, "\"");
	print(path + " " + param + "\n");
	
	sys.shell_execute(path, param);
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
}

function on_accel(accel) {
	print(accel + "\n");
}

function find_editer(ext) {
	print("ext " + ext + "\n");
	for(var i = 0; i < editer.length; ++i) {
		if(editer[i].ext.toLowerCase() == ext)
			return editer[i].editer;
	}
	
	return editer.length == 0 ? null : editer[0].editer;
}

function edit_file() {
	for(var i = 0; i < selected_files.length; ++i) {
		var path = new jpath(selected_files[i]);
		var edtr = find_editer(path.ext().toLowerCase());
		if(edtr == null)
			continue;
		
		sys.shell_execute(edtr, selected_files[i]);
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

function show_tooltip(ctrl, info) {
	var rc = ctrl.screen_rect();
	tooltip.move(rc.x, rc.y + rc.height + 1, tooltip.width, tooltip.height);
	tooltip.tip.text = info;
	tooltip.show();
}
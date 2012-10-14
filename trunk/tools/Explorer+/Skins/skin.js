var folder_index = 0;
var xplorer_left_pos = 0;
var curr_view_id;

$(function(){
	print("load succeeded\n");
	
	var obj = sys.explorer.drives();
	
	// ����drivebarʵ���������
	var width = 7 + obj.drives.length * 29 + obj.drives.length + 5;
	
	// �ƶ�������
	var offset = drivebar.width - width;
	drivebar.move(drivebar.x, drivebar.y, drivebar.width - offset, drivebar.height);
	addrbar.move(addrbar.x - offset, addrbar.y, addrbar.width + offset, addrbar.height);
	
	for(var i = 0; i < obj.drives.length; ++i) {
		var drv = obj.drives[i];		
		var percent = parseInt((drv.total - drv.free) / drv.total * 10);
		
		var id = "id='drive" + i + "' "; 
		var text = "text='" + drv.drive + "' ";
		var path = "path='" + drv.drive +":\\' ";
		var pos = "pos='" + percent + "' ";
		//var icon = "icon='icons." + drv.type + "' ";
		var icon = "icon='" + drv.drive + ":\\' ";
		var down = "down='" + (percent >= 9 ? "progress_hover_red.png" : "progress_hover.png");
		var item = "<item " + id + text + path + pos + icon + down + "' />";
		drivebar.insert(item);
	}
	
	sys.explorer.handler({
		open:function(path, hwnd, oldwnd) {
		}
	});
});

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
    if (!file.open(app_path + "\\Skins\\bookmark.json", "r"))
		return;
		
	var data = file.read();
	var bm = eval("(" + data + ")");
	var sys_path = sys.get_path(sys.sys_path);
	for (var i = 0; i < bm.bookmark.length; ++i) {
		var path = bm.bookmark[i].path;

		path = path.replace(/%Sys/gi, sys_path);
		path = path.replace(/%App/gi, app_path);

		var id = sys.hash(path);
		fav_toolbar.insert("<item id='btn" + id + "' path='" + path + "' icon='" + path + "|0|large' />", -1, 0);
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

function add_folder() {
	++folder_index;

	// ����һ����ͼ
	var view_id = "view" + folder_index;
	xplorer.views.insert("<item id='." + view_id + "' />");
	
	curr_view_id = "xplorer.views." + view_id;
    var view = eval(curr_view_id);
	var hid = sys.explorer.new(view.handler(), "c:\\windows", view.rect(), curr_view_id);
	
	// ����һ��tab��ť
	 var tab_id = "tab" + folder_index;
	 xplorer.tabs.insert("<item id='." + tab_id + "' tab='" + view.id + "' text='Windows Download' icon='C:\\Windows\\explorer.exe' />");
	 if(folder_index == 1)
		xplorer.tabs.tab1.check(true);
}

function open_folder(path) {
	addr.path = path;
	sys.explorer.open(0, path);
}
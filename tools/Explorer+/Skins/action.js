var frm_index = 0;
var view_index = 0;

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
    if (file.open(app_path + "\\Skins\\Explorer\\bookmark.json", "r")) {
        var sys_path = sys.get_path(sys.sys_path);

        var data = file.read();
        var bm = eval("(" + data + ")");
        for (var i = 0; i < bm.bookmark.length; ++i) {
            var path = bm.bookmark[i].path;

            path = path.replace(/%Sys/gi, sys_path);
            path = path.replace(/%App/gi, app_path);

            var id = sys.hash(path);
            fav_toolbar.insert("<item id='btn" + id + "' icon='" + path + "' />", -1, 0);
        }
    }

    file = null;
}

function add_folder() {
    ++frm_index;

    var frm_id = "frm" + frm_index;
    xplorer.frms1.frms.insert("<item id='." + frm_id + "' />");
    var frm = eval("xplorer.frms1.frms." + frm_id);

    var tab_id = "tab" + frm_index;
    xplorer.frms1.tabs.insert("<item id='." + tab_id + "' text='windows" + frm_index + "' tab='" + frm.id + "'/>");

    add_sub_folder(frm);
}

function add_sub_folder(frm) {
    if (typeof (frm) == "undefined") {
        return;
    }

    ++view_index;

    var view_id = "view" + view_index;
    frm.views.insert("<item id='." + view_id + "'/>");

    var subtab = "tab" + view_index;
    view_id = frm.views.id + "." + view_id;
    frm.tabs.insert("<item id='" + subtab + "' tab='" + view_id + "' text='system" + view_index + "' />");

    var jexplr = new jexplorer();
    jexplr.new(11000, "c:\\windows");
}
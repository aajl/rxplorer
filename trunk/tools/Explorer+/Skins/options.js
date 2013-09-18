var editor_backup = [];
var viewer_backup = [];

function on_ok_options() {
	options.hide();
	
	// apply changes.
	editor = editor_backup.concat();
	save2file(editor, "editor.json");
	
	viewer = viewer_backup.concat();
	save2file(viewer, "viewer.json");
}

function get_filters_exe() {
	var typename = extension.typename(".exe");
	var filters = typename + "(*.exe)|*.exe||";
	return filters;
}

function delete_ctrl_item(ctrl, item_id) {
	var ids = item_id.split(".");
	ids.splice(ids.length - 1, 1);
	item_id = ids.join(".");
	ctrl.remove(item_id);
}

function show_editor_viewer() {
	//////////////////////////////////////////////////////////
	// show editors
	var save = false;
	edit_view_options.editor.list.set_redraw(false);
	edit_view_options.editor.list.clear();
	for(var i = 0; i < editor.length; ++i) {
		var typename = editor[i].typename;
		var name = insert_editor_viewer("editor", editor[i].editor, editor[i].ext, typename);
		if(name.length > 0) {
			save = true;
			editor[i].typename = name;
		}
	}
	
	edit_view_options.editor.list.set_redraw(true);
	
	// save typename to file.
	if(save)
		save2file(editor, "editor.json");

	// backup editor for restore.
	editor_backup = editor.slice(0);

	//////////////////////////////////////////////////////////
	// show viewers
	save = false;
	edit_view_options.viewer.list.set_redraw(false);
	edit_view_options.viewer.list.clear();
	for(var i = 0; i < viewer.length; ++i) {
		var typename = viewer[i].typename;
		var name = insert_editor_viewer("viewer", viewer[i].viewer, viewer[i].ext, typename);
		if(name.length > 0) {
			save = true;
			viewer[i].typename = name;
		}
	}
	
	edit_view_options.viewer.list.set_redraw(true);
	
	// save typename to file.
	if(save)
		save2file(viewer, "viewer.json");
	
	// backup viewer for restore.
	viewer_backup = viewer.slice(0);
}

function add_editor_viewer(attr_name) {
	var path = sys.dialog_open(get_filters_exe());
	if(path.length == 0)
		return;
	
	// insert temp item.
	var ext = ".unknow";
	var id = sys.hash(ext + attr_name + "_ext");
	var item = {"id": "lyer" + id, "ext": ext, "desc": ""};
	item[attr_name] = path;
	
	var lst = eval("edit_view_options." + attr_name + ".list");
	lst.insert(item);
	
	// show edit ctrl.
	var btn = eval("lyer" + id + ".edit");
	btn.click();
	btn.hide();
	
	var edt = eval("lyer" + id + ".change");
	edt.text = "";
	edt.click();
}

function insert_editor_viewer(attr_name, attr_value, ext, typename) {
	var name = "";
	if(typeof(typename) == "undefined") {
		typename = extension.typename(ext);
		name = typename;
	}

	typename = ext + " (" + typename + ")";
	var id = sys.hash(typename);
	var item = {"id": attr_name + "_lyer_" + id, "ext": ext, "desc": typename};
	item[attr_name] = attr_value;
	
	var lst = eval("edit_view_options." + attr_name + ".list");
	lst.insert(item);
	
	return name;
}

function delete_editor_viewer(attr_name, bkup, ext, item_id) {
	var lst = eval("edit_view_options." + attr_name + ".list");
	if(ext == ".unknow") {
		delete_ctrl_item(lst, item_id);
		return;
	}
	
	for(var i = 0; i < bkup.length; ++i) {
		if(bkup[i].ext == ext) {
			delete_ctrl_item(lst, item_id);
			bkup.splice(i, 1);
			break;
		}
	}
}

function change_editor_viewer_path(attr_name, bkup, lyer_id, ext) {
	for(var i = 0; i < bkup.length; ++i) {
		if(bkup[i].ext == ext) {
			var pathname = sys.dialog_open(get_filters_exe());
			if(pathname.length > 0)
				bkup[i][attr_name] = pathname;
				
			break;
		}
	}
}

function show_edit_ctrl(lyer_id, ext) {
	var btn = eval(lyer_id + ".edit");
	btn.hide();
	
	var edt = eval(lyer_id + ".change");
	edt.text = ext;
	edt.show();
}

function change_editor_viewer_ext(attr_name, bkup, lyer_id, ext) {
	var edt = eval(lyer_id + ".change");
	edt.hide();
	
	var lyer = eval(lyer_id);
	var new_ext = edt.text;
	if(ext == ".unknow") {
		// get editor
		var edtr = eval(lyer_id + "." + attr_name);
		var attr_value = edtr.text;
		
		// delete temp item
		var lst = eval("edit_view_options." + attr_name + ".list");
		lst.remove(lyer.id);
		if(new_ext.length == 0 || new_ext == ".unknow")
			return;
		
		// insert new item
		var typename = insert_editor_viewer(attr_name, attr_value, new_ext);
		if(typename.length > 0) {
			var edr = {};
			edr.ext = new_ext;
			edr[attr_name] = attr_value;
			edr.typename = typename;
			bkup.push(edr);
		}
	}
	
	lyer.redraw();
}
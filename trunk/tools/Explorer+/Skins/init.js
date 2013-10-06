// 32. ��ʾ���������������Թ�����ļ���ʱ,�����ַ���������˵�����Ч(����).
// 34. ��������Ĺ��������ƶ�ʱ,�������Ƶ����ļ���ͼ��,������������ƶ�(����).
// 42. ���ĳһδ���ļ���ʱ,�������̫��,��δ��������ٵ����һ�ļ���,��ᵼ����ʾ����.()
// 9. �˵���������

$(function(){
	print("version: " + ver.version + "\n");
	
	sys.search.scan();

	var buylink = "https://shopper.mycommerce.com/checkout/cart/new/49846-3?quantity=1&locale=en_US&currency=USD";
	about.buy.hyperlink = buylink;
	about.expired.hyperlink = buylink;
	
	sys.search.handler({
		scan:function(operate, drive) {
			if(drive == 0 || drive == "0")
				searcher.status.text = operate;
			else
				searcher.status.text = operate + " " + String.fromCharCode(drive) + ":";
		},
		reload:function(operate) {
			searcher.status.text = "updating: " + operate;
		},
		search:function(count, completed) {
			if(Number(completed)) {
				searcher.btn.enable(true);
				searcher.status.text = count + " objects";
			} else {
				searcher.status.text = "searching";
			}
		},
	});
	
	sys.updater.handler({
		on_check:function(version, details) {
			about.update.enable(false);
			if(version == "") {
				about.update.text = lang.get("last_version");
			} else {
				new_version = version;
				about.update.text = lang.get("new_version") + version;
				sys.updater.download();
			}
		},
		on_download:function(path, readsize, filesize, index, count) {
			var percent = parseInt(100 / count * (index - 1) + readsize / filesize * (100 / 3));
			about.update.text = lang.get("new_version") + new_version + ", " + lang.get("downloading") + percent + "%";
			print(path + " -> readsize: " + readsize + ", filesize: " + filesize + " " + index + " " + count + "\n");
			if(percent == 100) {
				about.update.enable(true);
				about.update.text = lang.get("new_version") + new_version + ", " + lang.get("download_completed");
			}
		},
		on_update:function() {
		},
	});
	
	sys.explorer.handler({
		open:function(path, display_name, view_id) {
			var view = eval(view_id);
			var tab = eval(view.tabobj);

			tab.text = display_name;
			tab.path = path;
			tab.set_icon(path);
			
			if(curr_tab == tab) {
				tab.check(true);
				if(path.substr(0, 2) == "::" || path == "desktop")
					addr.path = display_name;
				else
					addr.path = path;
			}
			
			////curr_tab.text = display_name;
			////curr_tab.check(true);
			////curr_tab.path = path;
			////curr_tab.set_icon(path);
			////if(path.substr(0, 2) == "::" || path == "desktop")
			////	addr.path = display_name;
			////else
			////	addr.path = path;
			
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

			var view = eval(view_id);
			var tab = eval(view.tabobj);
						
			if(clicked_tab) {
				tab.text = display_name;
				tab.path = path;
				clicked_tab = false;
				filefilter.clear();
			} else {
				curr_tab = tab;
			}
			
			curr_tab = tab;

			print("path: " +  path + "\n");
			if(view_id.split(".", 1) == "xplorer") {
				pane1_curr_tab = curr_tab;
			}
			else if(view_id.split(".", 1) == "xplorer2") {
				pane2_curr_tab = curr_tab;
			}
			
			sys.explorer.get_file_types();
			addrbar.addrbar.sysfolder.set_icon(path);

			session_open = true;
			if(!session2_open) {
				session2_open = true;
				load_session("xplorer2", "session2.json");
			} else {
				if(!setting.explorer.dual)
					show_dual_pane(false);
				
				if(!opened) {
					opened = true;
					if(!sys.register.registered())
						about.show();
				}
			}
		},
		selected:function(files, count) {
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
					filesize += format_disk_size(pth.filesize());
				}
				
				if(setting.statusbar.show) { // ��ʾ��״̬�������Ϣ
					var fileinfo = pth.display_name();
					fileinfo += "    (" + pth.filetype() + ")";
					fileinfo += "    " + filesize;
					fileinfo += "    " + lang.get("create_time") + ": " + pth.modify_time();
					fileinfo += "    " + lang.get("modify_time") + ": " + pth.create_time();
					statusbar.filename.text = fileinfo;
				} else { // ��ʾ��ϸ״̬�������Ϣ
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
					statusbar.filename.text = "ѡ���� " + count + " ���ļ�";
				} else {
					detailbar.info.clear();
					detailbar.info.insert({"text": "ѡ���� " + count + " ���ļ�"});
				}
			}
		},
		selected_count:function(count) {
			if(setting.statusbar.show) {
				if(count > 0)
					statusbar.filename.text = "ѡ���� " + count + " ���ļ�";
				else
					statusbar.filename.text = "";
			} else {
				detailbar.info.clear();
				if(count > 0)
					detailbar.info.insert({"text": "ѡ���� " + count + " ���ļ�"});
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
						filefilter.insert({"text":"��ʾ�ļ���", "icon":"icons.folder", "ext": types[i], "id": ".filterfolder", "check":typeof(check) == "undefined" ? true : check});
					else if(types[i] == "#2hidden")
						filefilter.insert({"text":"��ʾ�����ļ�", "icon":"icons.hiddenfile", "ext": types[i], "id": ".filterhidden", "check":typeof(check) == "undefined" ? true : check});
					else if(types[i] == "#3file")
						filefilter.insert({"text":"��ʾ�����ļ�", "icon":"icons.allfile", "ext": types[i], "id": ".filterfile", "check":typeof(check) == "undefined" ? true : check});
					else
						filefilter.insert({"text":types[i], "icon":types[i], "ext": types[i], "id": ".filter" + sys.hash(types[i]), "check":check});
				}
				filefilter.set_redraw(true);
				filefilter.redraw();
			}
			
			fastfilter.text = sys.explorer.get_search_filter();
		},
		drives:function(drives) {
			drivebar.clear();
			insert_drives(drives, true);
		},
		dbclk:function() {
			up();
		},
		gesture:function(gesture, view_id) {
			var view = eval(view_id);
			var xplr_id = view_id.split(".", 1);
			var xplr = eval(String(xplr_id));
			if(gesture == "DR") 	 	// ���� --> �ر�
				close_tab(xplr, curr_tab);
			else if(gesture == "LU") 	// ���� --> �ָ��رյı�ǩ
				;
			else if(gesture == "DU")	// ���� --> ���Ƶ�ǰ·����������
				copy_curr_path();
			else if(gesture == "RD")	// ���� --> �½���ǩ
				new_tab_view(get_curr_xplorer());
			else if(gesture == "U") 	// �� --> ����
				up();
			else if(gesture == "L") 	// �� --> ����
				backward();
			else if(gesture == "R") 	// �� --> ǰ��
				forward();
			else if(gesture == "RDR")	// ������ --> ��һ����ǩ
				;
			else if(gesture == "LDL")	// ������ --> ��һ����ǩ
				;
			else if(gesture == "LDR")	// ������ --> ��windows�Ĺ�������
				open_curr_folder();
			else if(gesture == "UDU")	// ������(N) --> �½��ļ���
				new_folder();
				
			print("����: " + gesture + "\n");
		},
	});

	var drives = sys.explorer.drives();
	
	// ����drivebarʵ��������
	var width = 7 + drives.length * 29 + drives.length + 5;
	
	// �ƶ�������
	var offset = drivebar.width - width;
	drivebar.move(drivebar.x, drivebar.y, drivebar.width - offset, drivebar.height);
	addrbar.move(addrbar.x - offset, addrbar.y, addrbar.width + offset, addrbar.height);
	insert_drives(drives, true);

	sys.explorer.treeview_new("treeview.tree.view", treeview.tree.view.rect());
	
	load_setting();
	
	load_session("xplorer", "session.json");
	
	if(setting.statusbar.show)
		show_statusbar(true);

	if(!setting.treeview.show)
		show_treeview(false);
	
	if(setting.explorer.max)
		explorer.max();
	
	editor = json_from_file("editor.json", []);
	for(var i = 0; i < editor.length; ++i) {
		editor[i].editor = editor[i].editor.replace(/%Sys/gi, sys.path.system);
		editor[i].editor = editor[i].editor.replace(/%App/gi, sys.path.app);
	}
	
	viewer = json_from_file("viewer.json", []);
	for(var i = 0; i < viewer.length; ++i) {
		viewer[i].viewer = viewer[i].viewer.replace(/%Sys/gi, sys.path.system);
		viewer[i].viewer = viewer[i].viewer.replace(/%App/gi, sys.path.app);
	}

	sys.shell_execute(sys.path.app + "/search.exe",  "-app -mutex search_helper_mutex -Module Modules/MSearch.dll", "on_scan_completed");
});

var timer_play_pos = 0;
var show_cursor = true;
var prev_cursor_x = 0;
var prev_cursor_y = 0;
var cursor_count = 0;
var media_file_path = "";
var is_playing = false;
var repeat = false;
var playinfo = {};

Object.prototype.toJSON = function(){  
    var json = [];  
    for(var i in this){  
        if(!this.hasOwnProperty(i)) continue;  
        json.push(  
            i.toJSON() + " : " +  
            ((this[i] != null) ? this[i].toJSON() : "null")  
        )  
    }  
    return "{\n " + json.join(",\n ") + "\n}";  
}

Array.prototype.toJSON = function(){  
    for(var i=0,json=[];i<this.length;i++)  
        json[i] = (this[i] != null) ? this[i].toJSON() : "null";  
    return "["+json.join(", ")+"]"  
}

String.prototype.toJSON = function(){  
    return '"' +  
        this.replace(/(\\|\")/g,"\\$1")  
        .replace(/\n|\r|\t/g,function(){  
            var a = arguments[0];  
            return  (a == '\n') ? '\\n':  
                    (a == '\r') ? '\\r':  
                    (a == '\t') ? '\\t': ""  
        }) +  
        '"'
}

$(function(){
	// 构造播放信息的路径
	var path = sys.get_path(sys.app_path);
	path +=  "\\playinfo.json";
	var file = new jfile;
	if(!file.open(path, "r"))
		return;

	// 读取播放信息,构造json对象
	var play_info_json = file.read();
	playinfo = eval('(' + play_info_json + ')');
});

function on_play()
{
	if(media_file_path == "")
		open_file();
	else if(is_playing)
		sys.player.pause()
	else
		play_file(media_file_path);
}

function on_close()
{
	// 保存播放位置信息
	var path = sys.get_path(sys.app_path);
	path +=  "\\playinfo.json";
	var file = new jfile;
	if(file.open(path, "w"))
	{
		file.write(playinfo.toJSON());
	}

	dplayer.close();
}

function open_file()
{
	var filter = "RealMeida Files(*.rmvb)|*.rmvb|" +
				 "AVI Files(*.avi)|*.avi|" +
				 "MKV Files(*.mkv)|*.mkv|" +
				 "All Files(*.*)|*.*||";
	var filepath = sys.dialog.open(filter);
	play_file(filepath);
}

function play_file(filepath)
{
	show_cursor = true;
	prev_cursor_x = 0;
	prev_cursor_y = 0;
	
	if(sys.player.play(filepath))
	{
		media_file_path = filepath;
		is_playing = true;
		
		logo.hide();
		play_progress.enable(true);
		set_video_pos();
		openfile.hide();
		play_time.show();
		play.check(true);
		var duration = sys.player.duration();
		if(duration == 0)
		{
			play_progress.set_range(0, 100);
			footer_play_progress.set_range(0, 100);
		}
		else
		{
			play_progress.set_range(0, duration);
			footer_play_progress.set_range(0, duration);
		}
		
		filename.text = filepath.substr(filepath.lastIndexOf('\\') + 1);
		timer_play_pos = sys.setInterval(1000, 'on_play_pos()');
		
		var pos = playinfo[media_file_path];
		if(pos != 0 && pos != "undefined")
		{
			sys.player.set_play_pos(pos);
		}
	}
}

function stop_play()
{
	var full = sys.player.get_fullscreen();
	if(full)
		sys.player.set_fullscreen(Number(!full));

	play_time.hide();
	filename.text = "";
	logo.show();
	play.check(false);
	openfile.show();
	play_progress.pos = 0;
	footer_play_progress.pos = 0;
	play_progress.enable(false);
	footer_ctrl_bar.hide();
	footer.show();
	sys.player.stop();
	
	is_playing = false;
	
	sys.clearInterval(timer_play_pos);
}

function set_video_pos()
{
	sys.player.set_video_pos(videowindow.x, videowindow.y, videowindow.width, videowindow.height);
}

function set_play_pos(play_prgrs)
{
	var duration = sys.player.duration();
	if(duration == 0)
		return;
	
	sys.player.set_play_pos(play_prgrs.pos);
}

function on_play_pos()
{
	var duration = sys.player.duration();
	if(duration == 0)
		return;
				
	var full = sys.player.get_fullscreen();
	if(full)
	{
		var pt = sys.cursor_pos();
		if(prev_cursor_x == pt.x && prev_cursor_y == pt.y)
		{
			if(show_cursor && ++cursor_count == 5)
			{
				show_cursor = false;
				sys.show_cursor(false);
				footer_ctrl_bar.hide();
			}				
		}
		else
		{
			cursor_count = 0;
			prev_cursor_x = pt.x;
			prev_cursor_y = pt.y;
		}
	}
	
	var playpos = sys.player.get_play_pos();
	play_progress.pos = playpos;
	footer_play_progress.pos = playpos;
	playinfo[media_file_path] = playpos;

	function fmt(tt)
	{
		return tt < 10 ? '0' + tt : tt;
	}

	var hh = parseInt(playpos / 3600);
	var mm = parseInt((playpos / 60) % 60);
	var ss = parseInt(playpos % 60);
	var text = fmt(hh) + ":" + fmt(mm) + ":" + fmt(ss) + " / ";
	hh = parseInt(duration / 3600);
	mm = parseInt((duration / 60) % 60);
	ss = parseInt(duration % 60);
	text += fmt(hh) + ":" + fmt(mm) + ":" + fmt(ss);
	play_time.text = text;
	footer_play_time.text = text;
	
	if(duration == playpos)
	{
		stop_play();
		sys.clearInterval(timer_play_pos);
		delete playinfo[media_file_path];
		
		if(repeat)
			play_file(media_file_path);
	}
}

function show_ctrl_bar()
{
	if(show_cursor)
		return;
		
	var pt = sys.cursor_pos();
	if(prev_cursor_x == pt.x && prev_cursor_y == pt.y)
		return;

	cursor_count = 0;
	show_cursor = true;
	prev_cursor_x = 0;
	prev_cursor_y = 0;
	sys.show_cursor(true);
	footer_ctrl_bar.show();
}

function change_volume(volume_ctrl)
{
	var pos = volume_ctrl.pos;
	sys.player.set_volume(pos);
}

function move_main_menu()
{
	if(menu.popuped)
		return;

	var rc_menu_btn = menu.screen_rect();
	var rc_main_menu = main_menu.rect();
	var top = rc_menu_btn.top + rc_menu_btn.height + 4;
	main_menu.move(rc_menu_btn.left, top, rc_main_menu.width, rc_main_menu.height);
}

function drop_files(files)
{
	if(files.length == 0)
		return;
		
	play_file(files[0]);
}

function fullscreen()
{
	var full = !sys.player.get_fullscreen();
	if(!sys.player.set_fullscreen(Number(full)))
		return;
		
	if(full)
	{
		var rc_ctrl_bar = footer_ctrl_bar.rect();
		var rc_player = dplayer.screen_rect();
		var left = rc_player.left;
		var top = rc_player.top + rc_player.height - rc_ctrl_bar.height;
		footer_ctrl_bar.move(left, top, rc_player.width, rc_ctrl_bar.height);			
		
		footer.hide();
		footer_ctrl_bar.show();
	}
	else
	{
		footer.show();
		footer_ctrl_bar.hide();
	}
}

function show_media_info()
{
	menu.popup();

	if(!sys.mediainfo.open(media_file_path))
		return;
	
	filefmt.text = sys.mediainfo.file_format();
	filerate.text = sys.mediainfo.vbitrate();
	duration.text = sys.mediainfo.duration();


	var vcodecinfo = sys.mediainfo.vcodec_id();
	var vcodec_info = sys.mediainfo.vcodec_info();
	var vcodec_profile = sys.mediainfo.vcodec_profile();
	
	if(vcodecinfo == "")
		vcodecinfo = vcodec_info;
	else if(vcodec_info != "")
		vcodecinfo += "/" + vcodec_info;
				
	if(vcodecinfo == "")
		vcodecinfo = vcodec_profile;
	else if(vcodec_profile != "")
		vcodecinfo += "/" + vcodec_profile;
	
	videofmt.text = sys.mediainfo.video_format();
	vcodec.text = vcodecinfo;
	vbitrate.text = sys.mediainfo.vbitrate();
	fps.text = sys.mediainfo.fps();
	resolution.text = sys.mediainfo.resolution();
	aspect_ratio.text = sys.mediainfo.aspect_ratio();
	
	var acodecinfo = sys.mediainfo.acodec_id();
	var acodec_info = sys.mediainfo.acodec_info();
	var acodec_profile = sys.mediainfo.acodec_profile();

	if(acodecinfo == "")
		acodecinfo = acodec_info;
	else if(acodec_info != "")
		acodecinfo += "/" + acodec_info;
	
	if(acodecinfo == "")
		acodecinfo = acodec_profile;
	else if(acodec_profile != "")
		acodecinfo += "/" + acodec_profile;

	audiofmt.text = sys.mediainfo.audio_format();
	acodec.text = acodecinfo;
	abitrate.text = sys.mediainfo.abitrate();
	channels.text = sys.mediainfo.channels();
	srate.text = sys.mediainfo.srate();
	bitdepth.text = sys.mediainfo.bit_depth();

	fileinfo.show()
}
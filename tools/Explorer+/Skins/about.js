function check_for_updates() {
	new_version = "";
	sys.updater.check("explorer", sys.path.app + '/Skins/version.xml');
	about.update.text = lang.get("checking_update");
	about.show()
}

function show_about() {
	about.update.text = "";
	about.show()
}

function on_show_about() {
	var registered = sys.register.registered("", "debug");
	
	about.email.show(!registered);
	about.mail.show(!registered);
	about.license.show(!registered);
	about.code.show(!registered);
	about.buy.show(!registered);
	about.register.show(!registered);
	about.ok.show(registered);
	
	if(registered) {
		about.trial.hide();
		about.expired.hide();

		var license = sys.register.license();
		if(typeof(license.email) != "undefined") {
			about.user.text = license.email;
		}
	} else {		
		about.user.text = lang.get("unreg");
		
		var expired = sys.register.expired("", 30);
		about.trial.show(expired > 0);
		about.expired.show(expired == 0);
		
		if(expired > 0) {
			var trial = lang.get("trial");
			trial = trial.replace("%td", 30);
			trial = trial.replace("%ld", expired);
			about.trial.text = trial;
		}		
	}
}

function on_hide_about() {
	if(sys.register.expired("", 30) == 0)
		explorer.close();
}

function on_check_for_update(version, detail) {
}

function on_register() {
	if(about.mail.text == "") {
		register.alert("Email can not be null", "Error");
		return;
	}
	
	if(about.code.text == "") {
		register.alert("Licnese code can not be null", "Error");
		return;
	}
	
	if(sys.register.register(about.mail.text, about.code.text))
		about.hide();
}

function restart() {
}
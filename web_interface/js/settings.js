var settingsJson = {};

function load() {
	getFile("settings.json", function (res) {
		settingsJson = JSON.parse(res);
		// alert(settingsJson + "\n" + res);
		draw();
	});
}

function draw() {
	var html = "";
	for (var key in settingsJson) {
		key = esc(key);
		var _language_key = "lang";
		if (settingsJson.hasOwnProperty(key)) {
			html += "<div class='row'>" +
				"<div class='col-6'>" +
				"<label class='settingName " + (typeof settingsJson[key] == "boolean" ? "labelFix" : "") + "' for='" + key + "'>" + key + ":</label>" +
				"</div>" +
				"<div class='col-6'>";

			if (typeof settingsJson[key] == "boolean") {
				html += "<label class='checkBoxContainer'><input type='checkbox' name='" + key + "' " + (settingsJson[key] ? "checked" : "") + " onchange='save(\"" + key + "\",!settingsJson[\"" + key + "\"])'><span class='checkmark'></span></label>";
			} else if (typeof settingsJson[key] == "number") {
				html += "<input type='number' name='" + key + "' value=" + settingsJson[key] + " onchange='save(\"" + key + "\",parseInt(this.value))'>";
			} else if (typeof settingsJson[key] == "string" && key != _language_key) {
				var _value = settingsJson[key];
				console.log(_value)
				html += "<input type='text' name='" + key + "' value='" + _value + "' " + (key == "version" ? "readonly" : "") + " onchange='save(\"" + key + "\",this.value)'>";
			} else if (typeof settingsJson[key] == "string" && key == _language_key) {
				var _lang = settingsJson[key];
				// console.log(_lang)
				// alert();
				// html += "<input type='text' name='" + key + "' value='" + _value + "' " + (key == "version" ? "readonly" : "") + " onchange='save(\"" + key + "\",this.value)'>";

				html += "<select id='ch' name='ch' onchange='save(\"" + key + "\",this.value)'>" +
					'<option value="cn" ' + (_lang == "cn" ? "selected" : "") + '>Chinese</option>' +
					'<option value="cs" ' + (_lang == "cs" ? "selected" : "") + '>Czech</option>' +
					'<option value="de" ' + (_lang == "de" ? "selected" : "") + '>German</option>' +
					'<option value="en" ' + (_lang == "en" ? "selected" : "") + '>English</option>' +
					'<option value="es" ' + (_lang == "es" ? "selected" : "") + '>Spanish</option>' +
					'<option value="fi" ' + (_lang == "fi" ? "selected" : "") + '>Finnish</option>' +
					'<option value="fr" ' + (_lang == "fr" ? "selected" : "") + '>French</option>' +
					'<option value="it" ' + (_lang == "it" ? "selected" : "") + '>Italian</option>' +
					'<option value="ro" ' + (_lang == "ro" ? "selected" : "") + '>Romanian</option>' +
					'<option value="ru" ' + (_lang == "ru" ? "selected" : "") + '>Russian</option>' +
					'<option value="tlh" ' + (_lang == "tlh" ? "selected" : "") + '>Klingon</option>' +
					'<option value="vi" ' + (_lang == "vi" ? "selected" : "") + '>Vietnamese</option>' +
					'</select>'
			}



			html += "</div>" +
				"</div>" +
				"<div class='row'>" +
				"<div class='col-12'>" +
				"<p>" + lang("setting_" + key) + "</p>" +
				"<hr>" +
				"</div>" +
				"</div>";
		}
	}

	getE("settingsList").innerHTML = html;

}

function save(key, value) {
	if (key) {
		settingsJson[key] = value;
		getFile("run?cmd=set " + key + " \"" + value + "\"");
	} else {
		getFile("run?cmd=save settings", function (res) {
			load();
		});
	}
}
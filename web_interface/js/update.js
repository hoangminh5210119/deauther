var scanJson = {};

// loadLang();
function load() {
    getFile("run?cmd=save scan", function () {
        getFile("scan.json", function (res) {
            scanJson = JSON.parse(res);
            draw();
        });
    });
}

function draw() {
    var html;
    html = '<select id="ssid_update" name="ssid">';
    for (var i = 0; i < scanJson.aps.length; i++) {
        html += '<option value="' + esc(scanJson.aps[i][0]) + '">' + esc(scanJson.aps[i][0]) + '</option>'
    }
    html += '</select>';
    getE("ssid_update").innerHTML = html;
}


function connectWifi() {
    var current_ssid = getE("ssid_update").value;
    var password = getE("wifi_connect_password").value;

    var _ssid = "\"" + current_ssid + "\""
    var _password = "\"" + password + "\""
    var cmd = "run?cmd=connect " + _ssid + " " + _password

    if (password.length < 8) {
        alert(lang("password_error"));
    } else {
        getFile(cmd);
    }
}


var updateFirmware = {};
function getUpdateAvailable() {
    // alert("a");
    getFile("firmware.json", function (res) {
        updateFirmware = JSON.parse(res);
        drawUpdateFirmware();
    });
}

function drawUpdateFirmware() {
    var html;
    // Names
    getE("updateNum").innerHTML = updateFirmware.length;
    html = "<tr>" +
        "<th class='id'></th>" +
        "<th class='name'>name</th>" +
        "<th class='version'>version</th>" +
        "<th class='type'>connect</th>" +
        "<th class='release'>Release</th>" +
        "<th class='update'></th>" +
        "</tr>";

    for (var i = 0; i < updateFirmware.length; i++) {
        // selected = nameJson[i][nameJson[i].length - 1];

        var name = updateFirmware[i].name.toLowerCase();
        var type = "";
        var release = "✓";
        var url = updateFirmware[i].url;
        if (name.includes("1-wire")) {
            type = "1-Wire";
            name = name.replace("1-wire", "");
        }
        if (name.includes("i2c")) {
            type = "I2C";
            name = name.replace("i2c", "");
        }

        if (name.includes("uart")) {
            type = "UART";
            name = name.replace("uart", "");
        }

        if (name.includes("analog")) {
            type = "Analog";
            name = name.replace("analog", "");
        }

        if (name.includes("test")) {
            release = "x"
            name = name.replace("test", "");
        }

        html += "<tr>" +
            "<td class='id'>" + i + "</td>" // ID
            +
            "<td class='mac' contentEditable='true' id='name_" + i + "_mac'>" + name + "</td>" // MAC
            +
            "<td class='version'>" + esc(updateFirmware[i].version) + "</td>" // Vendor
            +
            "<td class='type'  id='name_" + i + "_name'>" + type + "</td>" // Name
            +
            "<td class='release'  id='name_" + i + "_name'>" + release + "</td>" // Name
            +
            // "<td class='update'  id='name_" + i + "_name'>" + "<button class='red' onclick='update(" + i + ")'>lang('update')</button>" + "</td>"
            '<td class="update"  id="name_' + i + '_name">' + '<button class="red" value="update" onclick="update(' + i + ')">' + (lang("update") == "" ? "update" : lang("update")) + '</button>' + '</td>' // Name
        "</tr>";
    }

    getE("updateTable").innerHTML = html;
    // alert(lang("update"));
}



function update(id) {
    getFile("run?cmd=update " + id);
}
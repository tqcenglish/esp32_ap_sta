import { headers } from "../../../lib/utils.js";
function infoWifi() {
	return fetch("/api/wifi/info", {headers}).then(res => res.json())
}

function updateWifi() {
	return fetch("/api/wifi/config", {method: 'POST'}).then(res => res.json())
}

function updateNetwork() {
	return fetch("/manager/list", {headers}).then(res => res.json())
}

export {infoWifi, updateWifi, updateNetwork}
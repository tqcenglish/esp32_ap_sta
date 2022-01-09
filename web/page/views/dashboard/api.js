function infoWifi() {
	return fetch("/api/wifi/info", ).then(res => res.json())
}

function updateWifi(data) {
	return fetch("/api/wifi/config", {method: 'POST', body: JSON.stringify(data)}).then(res => res.json())
}

function updateNetwork() {
	return fetch("/manager/list", ).then(res => res.json())
}

export {infoWifi, updateWifi, updateNetwork}
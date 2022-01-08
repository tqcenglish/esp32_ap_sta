
const maxReadSize = 1024 * 1024 * 1024; // 1G

const prefixPath = "/home/hotstandby/data/gluster"
const hotstandbyFile = "/home/hotstandby/conf/hotstandby.conf"
const action = "/usr/share/cockpit/broadcast/bin/action.sh"
const restore = "/usr/share/cockpit/broadcast/bin/restore.sh";

function yyyMMdd() {
    return new Date().toLocaleDateString().split("/").reverse().join('-');
}

function apiFile() {
    return hotActive ? `${prefixPath}/broadcast-system/data/zycoo_init/api.conf` : `/home/broadcast-system/data/zycoo_init/api.conf`;
}

function listDir() {
    return hotActive ? `${prefixPath}/broadcast-backup-list` : `/home/broadcast-backup-list`;
}

function dockerDir(){
    return hotActive ? `${prefixPath}/broadcast-system` : `/home//broadcast-system`;
}

let hotActive = false;
function setHotActive(active) {
    if(active === 'yes'){
       hotActive = true;
       return;
    }
    hotActive = false;
    return;
}

function headers() {
    return {
        "Authorization": `Basic ${Base64.encode(localStorage.setItem(`loginType`) + ':' + localStorage.setItem(`loginValue`))}`
    }
}


export {
    yyyMMdd,
    setHotActive,
    apiFile,
    listDir,
    dockerDir,
    maxReadSize,
    action,
    restore,
    hotstandbyFile,
    headers,
}


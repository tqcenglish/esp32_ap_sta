
export default {
    common:{
        OK:"确认",
        Cancel:"取消",
        OperationFailed:"操作失败",
        OperationSuccessful:"操作成功",
        LoadFailed:"加载失败",
        Download:"下载",
        SelectBackupFile:"请选择备份文件!",
        ErrorBackupFile:"无效的备份文件!"
    },
    hot: {
        HotStandBy:'双机热备',
        Activate: '启用',
        Secret: '密钥',
        Role: '工作模式',
        Master: '主服务器',
        Backup: '备用服务器',
        Interface: '虚拟IP网络接口',
        DataInterface: '热备连接网络接口',
        MasterPeer: '主服务器IP地址',
        BackupPeer: '备用服务器IP地址',
        Vr_ip: '虚拟IP地址',
        Save_success: '保存成功!',
        Save_failed: '保存失败!',
        Status: '双机热备状态',
        Current: '广播应用',
        Select: '请选择',
        Status_tip: '双机热备服务当前的运行状态描述',
        Current_tip: '广播服务现在是否运行在本服务器上',
        Activate_tip: '双机热备服务的开启/关闭',
        Role_tip: '本服务器充当的主服务器或备服务器角色',
        Secret_tip: '用于主备服务器之间的连接认证,主备服务器密钥必须相同',
        DataInterface_tip: '用于双机热备服务的数据传输接口,心跳检测等等',
        BackupPeer_tip: '备机的热备服务通信IP地址',
        MasterPeer_tip: '主机的热备服务通信IP地址',
        Interface_tip: '新增的虚拟IP工作在此网络接口,请选择与局域网其他终端设备通信的网络接口,如电话机等等',
        Vr_ip_tip: '需要当前局域网中没有使用的IP地址，且该地址需与选择的网络接口的主IP地址处于同一子网中，主备机的虚拟IP需填写相同',
        check_conf_failed: '请检查你的配置信息!',
        check_conf_success: '配置信息核查成功!',
        wait_host_connect: '等待备机连接中...',
        backup_con_suc: '备机连接成功,切换服务中,请稍等...',
        success_access: '热备服务已初始化成功,请访问虚拟IP地址',
        auth_failed: '认证失败,请检查密钥是否一致!',
        conc_master: '连接主机中...',
        conc_master_success: '连接主机成功,备机初始化完成,服务中...',
        hot_disabled: '双机热备服务已关闭!',
        handle: '热备服务器正在处理中，请稍等...',
        not_subnet: '填写的虚拟IP地址和所选虚拟IP网络接口的主IP不在同一子网中，请重新填写虚拟IP地址!',
        yes: '在本机',
        no: '不在本机',
        unknown: '未知状态',
        Submit: '提交'
    },
    log: {
        Log:"日志",
        sip:"SIP",
        tip1:"下载今日系统日志消息,发送给售后排查问题。",
        tip2:"通过 sngrep 抓取 30s 的数据包(可能不能抓取到有效包), 发送给售后排查问题。",
    },
    multi:{
        Broadcast:"广播方式",
        HTTP:"HTTP 流",
        Multi:"组播流",
        tip1:"HTTP 支持公网, 组播流同步一致性好但是只能在局域网工作. 需要重启系统生效",
        Restart:"重启服务"
    },
    backup:{
        Backup:"备份",
        Date:"日期",
        FileName:"文件名",
        Size:"大小",
        Action:"操作",
        Restore:"恢复",
        Download:"下载",
        Delete:"删除",
        BackupData:"备份数据",
        ConfigurationFiles:"配置文件",
        MusicFile:"音乐文件",
        RecordFile:"录音文件",
        BackupTime:"自动备份",
        BackupManual:"手动备份",
        Upload:"上传",
        SelectFile:"选择备份文件",
        tip1:"开启自动备份功能后，系统将每天自动备份，并保留最近5天的自动备份文件。",
        tip2:"只能上传下载的备份文件",
        tip3:"备份文件包括的内容，默认包括音乐文件，录音文件，不能修改。",
        tip4:"系统在每天凌晨 2 点自动备份，备份超过 5 个循环删除。",
        tip5:"删除备份文件不可恢复,确认操作?",
        tip6:"应用恢复会覆盖已有数据,确认操作?"
    }
}

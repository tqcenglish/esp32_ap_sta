void setApDhcpInfo()
{   
	tcpip_adapter_ip_info_t  apNetConfInfo  = {0};
	//初始化网络和DHCP有关内存，必须要提前执行这个API 不然下面操作直接让设备重启
	// tcpip_adapter_init();  
	// 将AP模式的DHCP 模式先关闭
	tcpip_adapter_dhcps_stop ( TCPIP_ADAPTER_IF_AP );
	//将对应的地址转成对应的字节序， 要注意apNetConfInf成员是结构体嵌套结构体，才是数组
	//
	IP4_ADDR( &apNetConfInfo.ip      , 192 , 168 , 32   , 1);  //设置IP
	IP4_ADDR( &apNetConfInfo.gw      , 192 , 168 , 32   , 1);  //设置网关
	IP4_ADDR( &apNetConfInfo.netmask , 255 , 255 , 255 , 0);  //设置子网掩码
	
	tcpip_adapter_set_ip_info( TCPIP_ADAPTER_IF_AP , &apNetConfInfo ); //设置IP信息
	
	tcpip_adapter_dhcps_stop(  TCPIP_ADAPTER_IF_AP ); //重新启动AP模式 DHCP 
}
import { infoWifi, updateWifi, updateNetwork } from './api.js'
export default {
  template: `
<div>
  <el-card style="margin: 30px;">
    <div style="display: flex; justify-content: center;">
      <h2>网络配置</h2>
    </div>
    <el-row :gutter="20" style="margin:20px;">
      <el-col :span="11">
        <el-form :label-position="'left'" label-width="100px" :model="formData">
            <el-form-item><h2>设备配置</h2> </el-form-item>
            <el-form-item label="wifi 网络名">
              <el-input v-model="formData.ssid"></el-input>
            </el-form-item>
            <el-form-item label="wifi 密码">
              <el-input v-model="formData.passwd"></el-input>
            </el-form-item>
            <el-form-item label="设备编号">
              <el-input v-model="formData.serial"></el-input>
            </el-form-item>
            <el-form-item>
              <el-button type="primary" @click="updateWifi">提交</el-button>
            </el-form-item>
        </el-form>
      </el-col>

      <el-col :span="2" style="display: flex; justify-content: center;">
        <div style="height:300px;">
          <el-divider style="height: 300px;" direction="vertical"></el-divider>
        </div>
      </el-col>

      <el-col :span="11">
        <el-form :label-position="'left'" label-width="100px" :model="networkData">
            <el-form-item><h2>
              设备信息 
                <span v-if='!networkData.ip' style="color:red;">连接失败</span>
                <span v-if='networkData.ip' style="color:green;">连接成功</span>
              </h2> </el-form-item>
            <el-form-item label="IP">
              <el-input v-model="networkData.ip"  :disabled="true"></el-input>
            </el-form-item>
            <el-form-item label="网关">
              <el-input v-model="networkData.gw"  :disabled="true"></el-input>
            </el-form-item>
            <el-form-item label="子网掩码">
              <el-input v-model="networkData.netmask"  :disabled="true"></el-input>
            </el-form-item>
            <el-form-item label="mac">
              <el-input v-model="networkData.mac"  :disabled="true"></el-input>
            </el-form-item>
            <el-form-item>
            <!-- <el-button type="primary" @click="updateNetwork">更新</el-button> -->
            </el-form-item>
        </el-form>
      </el-col>
    </el-row>
  </el-card>

  <!--
  <el-card style="margin: 30px;">
    <div style="display: flex; justify-content: center;">
      <h2>其他配置</h2>
    </div>
    <el-row :gutter="20" style="margin:20px;">
      <el-col :span="12">
        <el-form :label-position="'left'" label-width="100px" :model="networkData">
          <el-form-item label="请求地址">
            <el-input v-model="networkData.ip"></el-input>
          </el-form-item>
          <el-form-item>
            <el-button type="primary" @click="updateNetwork">更新</el-button>
          </el-form-item>
        </el-form>
      </el-col>
    </el-row>
  </el-card>
  -->
</div>
`,
  data() {
    return {
      formData: {
        ssid:"",
        passwd:"",
      },
      networkData:{
        ip:"",
        gw:"",
        netmask:"",
      },
      handle:null,
      updateFormData: true,
    };
  },
  components: {
  },
  mounted() {
    this.infoWifi();
    this.handle = setInterval(this.infoWifi, 3000);
  },
  methods: {
    infoWifi() {
      infoWifi().then(res => {
        if(this.updateFormData){
          this.formData.ssid = res.ssid;
          this.formData.passwd = res.passwd;
          this.formData.serial = res.serial;
          this.updateFormData = false;
        }

        this.networkData.ip = res.ip;
        this.networkData.gw = res.gw;
        this.networkData.netmask = res.netmask;
        this.networkData.mac = res.mac;
      });
    },
    updateWifi(){
      updateWifi(this.formData).then(res => {
        console.log(res);
        this.$message.info("更新成功");
      }).catch(() => {
        this.$message.info("更新成功");
      })
    },
    updateNetwork(){
      updateNetwork().then(res => {
        console.log(res);
      })
    }
  },
  beforeDestroy(){
    if(this.handle){
      clearInterval(this.handle);
    }
  } 
}
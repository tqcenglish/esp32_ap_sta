import { infoWifi, updateWifi, updateNetwork } from './api.js'
export default {
  template: `
<div>
  <el-card style="margin: 30px;">
    <div style="display: flex; justify-content: center;">
      <h2>网络配置</h2>
    </div>
    <el-row :gutter="20" style="margin:20px;">
      <el-col :span="12">
        <el-form :label-position="'left'" label-width="100px" :model="formData">
            <el-form-item label="网络名">
              <el-input v-model="formData.ssid"></el-input>
            </el-form-item>
            <el-form-item label="密码">
              <el-input v-model="formData.passwd"></el-input>
            </el-form-item>
            <el-form-item>
              <el-button type="primary" @click="updateWifi">提交</el-button>
            </el-form-item>
        </el-form>
      </el-col>
    </el-row>
  </el-card>

  <el-card style="margin: 30px;">
    <div style="display: flex; justify-content: center;">
      <h2>网络信息</h2>
    </div>
    <el-row :gutter="20" style="margin:20px;">
      <el-col :span="12">
        <el-form :label-position="'left'" label-width="100px" :model="networkData">
          <el-form-item label="IP">
            <el-input v-model="networkData.ip"></el-input>
          </el-form-item>
          <el-form-item label="掩码">
            <el-input v-model="networkData.mark"></el-input>
          </el-form-item>
          <el-form-item label="网关">
            <el-input v-model="networkData.gateway"></el-input>
          </el-form-item>
          <el-form-item>
            <el-button type="primary" @click="updateNetwork">更新</el-button>
          </el-form-item>
        </el-form>
      </el-col>
    </el-row>
  </el-card>
</div>
`,
  data() {
    return {
      formData: {},
      networkData:{},
    };
  },
  components: {
  },
  mounted() {
    this.infoWifi()
  },
  methods: {
    infoWifi() {
      infoWifi().then(res => {
        this.formData = res;
      });
    },
    updateWifi(){
      updateWifi().then(res => {
        console.log(res);
      })
    },
    updateNetwork(){
      updateNetwork().then(res => {
        console.log(res);
      })
    }
  }
}
export default {
  template: `
      <div style="display:flex; height:100%;">
        <div style="width:240px; height: 100%;">
          <el-menu
            default-active="dashboard"
            background-color="#545c64"
            text-color="#fff"
            active-text-color="#ffd04b"
            router
            class="fill-height el-menu-vertical-demo"
            >
            <el-menu-item index="/main/dashboard">
              <i class="el-icon-menu"></i>
              <span slot="title">网络配置</span>
            </el-menu-item>

            <!--
            <el-submenu index="2">
              <template slot="title">
                <i class="el-icon-location"></i>
                <span>设置</span>
              </template>
              <el-menu-item-group>
                <template slot="title">系统设置</template>
                <el-menu-item index="/main/terminal">Terminal</el-menu-item>
              </el-menu-item-group>
            </el-submenu>

            <el-menu-item index="/login" style="color:red;">
              <i class="el-icon-remove" style="color:red;"></i>
              <span slot="title"> 注销 </span>
            </el-menu-item>
            -->

          </el-menu>
        </div>
        <div style="width:100%;">
          <router-view/>
        </div>
      </div>
    `,
  name: 'main_view',
  data() {
    return {
    };
  },
  components: {
  },
  methods: {
    update(collapse) {
      this.collapse = collapse;
    },
  },
}

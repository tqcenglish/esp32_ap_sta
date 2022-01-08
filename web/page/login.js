export default {
	template: `
	<div style="display:flex; justify-content:center;margin-top: 300px">
		<div style="width:420px; height:200px;">
			<el-form :model="formData">
				<el-form-item>
					<template>
						<div slot="label" style="line-height: 20px;">
							<div>请输入贴在设备上的贴标签里的MAC地址登陆,如"<span style="color:red;">68692EABCD42</span>"</div>
							<div>Please enter the MAC address on the sticker attached to the device, like "<span style="color:red;">68692EABCD42</span>".</div>
						</div>
					</template>
					<el-input v-model="formData.mac" placeholder="mac"></el-input>
				</el-form-item>
				<el-form-item>
					<el-button type="primary" @click="onSubmit">提交/Submit</el-button>
				</el-form-item>
			</el-form>
		</div>
	</div>
	`,
	data() {
		return {
			formData: {
				mac: '',
			}
		}
	},
	methods: {
		onSubmit() {
			localStorage.setItem(`loginType`, 'mac')
			localStorage.setItem(`loginValue`, this.formData.mac)

			this.$router.push("/main/dashboard")
			// fetch('/').then(res => {
			// 	this.$router.push("/main/dashboard")
			// });
		}
	}
}
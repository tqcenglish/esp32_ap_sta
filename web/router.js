import Main from "./page/main.js";
import login from "./page/login.js";
import Dashboard from "./page/views/dashboard/index.js"

const routes = [
	{
		path: '/main', component: Main,
		children: [
			{
				path: '/',
				redirect: '/main/dashboard'
			},
			{
				path: 'dashboard',
				component: Dashboard
			},
		]
	},
	{ path: '/login', component: login },
	{ path: '/', redirect: '/main' }
]

export default new VueRouter({
	routes
  });
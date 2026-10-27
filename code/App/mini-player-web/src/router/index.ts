// 路由表。对应 §03 Step 3.6：三 Tab 子路由。
import { createRouter, createWebHashHistory, type RouteRecordRaw } from 'vue-router'

const LibraryPage = () => import('@/ui/pages/LibraryPage.vue')
const PlayingPage = () => import('@/ui/pages/PlayingPage.vue')
const SettingsPage = () => import('@/ui/pages/SettingsPage.vue')

const routes: RouteRecordRaw[] = [
  { path: '/', redirect: '/library/all' },
  {
    path: '/library/:tab(all|audio|video|favorite|history)',
    name: 'library',
    component: LibraryPage,
    props: true,
  },
  { path: '/playing', name: 'playing', component: PlayingPage },
  { path: '/settings', name: 'settings', component: SettingsPage },
  { path: '/:pathMatch(.*)*', redirect: '/library/all' },
]

export default createRouter({
  // 用 hash history：静态托管也能跑，部署成本更低
  history: createWebHashHistory(),
  routes,
})

# Vue 2 to Vue 3 Migration Guide

## What Changed

### Build System
- **Removed**: Vue CLI (webpack-based)
- **Added**: Vite (ultra-fast build tool)
- Build is now **10-100x faster**

### State Management
- **Removed**: Vuex with class-based modules
- **Added**: Pinia (official Vue state management)
- Better TypeScript support and smaller bundle size

### API Style
- **Changed**: Class-based components (vue-class-component)
- **To**: Composition API with `<script setup>`
- More intuitive and performant

### Scripts
| Old | New |
|-----|-----|
| `npm run serve` | `npm run dev` |
| `npm run build` | `npm run build` |
| `npm run lint` | `npm run lint` |

### File Structure Updates
1. **Store modules** need to be converted from Vuex to Pinia
   - Update `src/store/modules/*.ts` files to use `defineStore`
   - Import and use stores with `useXxxxStore()`

2. **Components** should migrate from class to Composition API
   - Remove `@Component` decorators
   - Use `<script setup lang="ts">` instead
   - Use `ref`, `computed`, `watch` from Vue

3. **Vuetify 3** API changes
   - Theme system simplified
   - Use `useTheme()` composable instead of `this.$vuetify`

### Installation
```bash
cd web
npm install
npm run dev
```

### Build
```bash
npm run build
```

## Migration Checklist

- [x] Update package.json
- [x] Add Vite configuration
- [x] Update entry point (main.ts)
- [x] Update Router (vue-router v4)
- [x] Setup Pinia
- [x] Update Vuetify 3
- [x] Update App.vue to Composition API
- [ ] Migrate store modules to Pinia
- [ ] Migrate components to Composition API
- [ ] Test all functionality
- [ ] Update Docker build if needed

## Resources
- [Vue 3 Migration Guide](https://v3-migration.vuejs.org/)
- [Pinia Documentation](https://pinia.vuejs.org/)
- [Vuetify 3 Documentation](https://vuetifyjs.com/)
- [Vite Documentation](https://vitejs.dev/)

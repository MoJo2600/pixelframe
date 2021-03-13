<template>
  <v-navigation-drawer v-model="drawerVisible" app hide-overlay>
    <v-list nav>
      <template
        v-for="item in $router.options.routes.filter(r => r.path !== '/')"
      >
        <v-list-item v-if="!item.children" :key="item.name" :to="item.path">
          <v-list-item-icon>
            <v-icon>{{ item.meta.icon }}</v-icon>
          </v-list-item-icon>

          <v-list-item-content>
            <v-list-item-title>{{ item.name }}</v-list-item-title>
          </v-list-item-content>
        </v-list-item>

        <v-list-group
          v-else
          :group="item.path"
          :key="item.name"
          :prepend-icon="item.meta.icon"
        >
          <template #activator>
            <v-list-item-title>{{ item.name }}</v-list-item-title>
          </template>

          <v-list-item
            v-for="nestedItem in item.children"
            :key="nestedItem.name"
            :to="`${item.path}/${nestedItem.path}`"
            dense
          >
            <v-list-item-content>
              <v-list-item-title class="ml-2">{{
                nestedItem.name
              }}</v-list-item-title>
            </v-list-item-content>
          </v-list-item>
        </v-list-group>
      </template>
    </v-list>
  </v-navigation-drawer>
</template>

<script lang="ts">
import { Vue, Component, VModel } from "vue-property-decorator";

@Component
export default class Navigation extends Vue {
  @VModel()
  public readonly drawerVisible!: boolean;
}
</script>

<style lang="scss" scoped>
.v-list-item--active {
  color: var(--v-primary-base) !important;
}

.v-list-item--active::before {
  background: var(--v-primary-base);
}
</style>

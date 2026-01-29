<template>
  <v-row>
    <v-col cols="12">
      <v-card :disabled="error" elevation="0" outlined>
        <v-card-text>
          <v-row v-if="title">
            <v-col cols="12">
              <p class="text-h6">{{ title }}</p>
            </v-col>
          </v-row>

          <slot></slot>
        </v-card-text>
      </v-card>
    </v-col>
  </v-row>
</template>

<script setup lang="ts">
defineProps<{
  error: boolean
  title?: string
}>()
</script>

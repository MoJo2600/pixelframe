<template>
  <v-row>
    <v-col cols="12">
      <v-btn
        class="float-right"
        color="primary"
        type="submit"
        :disabled="loading || error || !formValid"
        :loading="!!writing"
        depressed
      >
        {{ title }}
      </v-btn>
    </v-col>
  </v-row>
</template>

<script setup lang="ts">
defineProps<{
  error: boolean
  loading: boolean
  formValid: boolean | null
  writing: string | null
  title: string
}>()
</script>

<template>
  <v-alert
    dismissible
    :type="notification.type"
    transition="scale-transition"
    :value="visible"
  >
    <p class="text-body-1">{{ notification.content }}</p>
    <span v-if="notification.details" class="text-caption">{{
      notification.details
    }}</span>
  </v-alert>
</template>

<script lang="ts">
import Vue from "vue";
import Component from "vue-class-component";
import { Prop } from "vue-property-decorator";
import { NotificationData } from "@/store/modules/notification";

@Component
export default class Notification extends Vue {
  private readonly notificationVisibleDuration = process.env
    .VUE_APP_NOTIFICATION_DURATION
    ? Number(process.env.VUE_APP_NOTIFICATION_DURATION)
    : 8000;

  @Prop({
    required: true
  })
  private notification!: NotificationData;

  private visible = true;

  created() {
    setTimeout(() => {
      this.visible = false;
    }, this.notificationVisibleDuration);
  }
}
</script>

<style lang="scss" scoped>
.v-alert__content,
.text-body-1 {
  margin: 0;
}
</style>

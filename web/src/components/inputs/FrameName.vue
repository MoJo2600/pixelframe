<template>
  <v-container>
    <configuration-input-wrapper
      title="Framename / Hostname"
      skeletonType="input"
      :error="error"
      :loading="loading"
    >
      <v-text-field
        v-if="!loading && !error"
        v-model="frameName"
        outlined
        :rules="[required]"
        hide-details
      ></v-text-field>
    </configuration-input-wrapper>
  </v-container>
</template>

<script lang="ts">
import Vue from "vue";
import Component from "vue-class-component";
import { Prop, VModel } from "vue-property-decorator";
import ConfigurationInputWrapper from "@/components/ConfigurationInputWrapper.vue";
import { required } from "@/validation";

@Component({
  components: {
    ConfigurationInputWrapper
  }
})
export default class FrameNameInput extends Vue {
  required = required;

  @VModel({ type: String })
  public frameName!: string;

  @Prop()
  public loading!: boolean;

  @Prop()
  public error!: boolean;
}
</script>

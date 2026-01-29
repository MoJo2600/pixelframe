import { createVuetify } from 'vuetify'
import * as components from 'vuetify/components'
import * as directives from 'vuetify/directives'
import { md3 } from 'vuetify/blueprints'

// Vuetify styles
import 'vuetify/styles'

export default createVuetify({
  blueprint: md3,
  components,
  directives,
  theme: {
    defaultTheme: 'light',
    themes: {
      light: {
        colors: {
          primary: '#4CAF50',
          secondary: '#00897B',
          accent: '#F44336',
          background: '#F5F5F5',
        },
      },
      dark: {
        colors: {
          primary: '#00BCD4',
          secondary: '#00838F',
          accent: '#F44336',
          background: '#121212',
        },
      },
    },
  },
})

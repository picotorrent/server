<template>
  <div>
    <h5 class="mt-2">Listen interfaces</h5>

    <p>These are the listen interfaces that PicoTorrent Server will bind to.</p>
    <div class="table-control">
      <table>
        <thead>
          <tr>
            <th scope="col">Host</th>
            <th scope="col">Port</th>
            <th scope="col" class="actions">Actions</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="li in listenInterfaces" :key="li.id">
            <td>{{ li.host }}</td>
            <td>{{ li.port }}</td>
            <td class="actions">
              <button class="remove red" title="Remove" @click="remove(li.id)"><i class="bi bi-trash"></i></button>
            </td>
          </tr>
        </tbody>
      </table>
    </div>

    <router-link to="/settings/connection/add-listen-interface" custom v-slot="{ navigate }">
      <button class="icon" @click="navigate">
        <i class="bi bi-plus-square"></i>
        Add listen interface
      </button>
    </router-link>


    <h5>Protocols</h5>

    <p>Enable/disable uTP or TCP. Adjust uTP parameters.</p>

    <div class="check">
      <input type="checkbox" id="enableIncomingTcp" v-model="enableIncomingTcp">
      <label for="enableIncomingTcp">Incoming TCP enabled</label>
    </div>

    <div class="check">
      <input type="checkbox" id="enableIncomingUtp" v-model="enableIncomingUtp">
      <label for="enableIncomingUtp">Incoming uTP enabled</label>
    </div>

    <div class="check">
      <input type="checkbox" id="enableOutgoingTcp" v-model="enableOutgoingTcp">
      <label for="enableOutgoingTcp">Outgoing TCP enabled</label>
    </div>

    <div class="check">
      <input type="checkbox" id="enableOutgoingUtp" v-model="enableOutgoingUtp">
      <label for="enableOutgoingUtp">Outgoing uTP enabled</label>
    </div>

    <button class="mt-1" @click="save">
      Save settings
    </button>
  </div>
</template>

<script>
export default {
  data () {
    return {
      listenInterfaces: [],
      enableIncomingTcp: null,
      enableIncomingUtp: null,
      enableOutgoingTcp: null,
      enableOutgoingUtp: null,
    }
  },
  async mounted () {
    const [
      listenInterfacesList,
      activeProfile
    ] = await this.$rpc([
      [ 'listenInterfaces.getAll' ],
      [ 'profiles.getActive' ]
    ]);

    this.listenInterfaces = listenInterfacesList;

    if (activeProfile) {
      const settingsPack = await this.$rpc('settingsPack.getById', [ activeProfile.settings_pack_id ]);

      if (settingsPack) {
        this.enableIncomingTcp = settingsPack.settings.enable_incoming_tcp;
        this.enableIncomingUtp = settingsPack.settings.enable_incoming_utp;
        this.enableOutgoingTcp = settingsPack.settings.enable_outgoing_tcp;
        this.enableOutgoingUtp = settingsPack.settings.enable_outgoing_utp;
      }
    }
  },
  methods: {
    async remove(id) {
      await this.$rpc('listenInterfaces.remove', [ id ]);
      this.listenInterfaces = await this.$rpc('listenInterfaces.getAll');
    },
    async save() {
      const activeProfile = await this.$rpc('profiles.getActive');

      if (activeProfile) {
        await this.$rpc('settingsPack.update', [
          activeProfile.settings_pack_id, {
            settings: {
              enable_incoming_tcp: this.enableIncomingTcp,
              enable_incoming_utp: this.enableIncomingUtp,
              enable_outgoing_tcp: this.enableOutgoingTcp,
              enable_outgoing_utp: this.enableOutgoingUtp
            }
          }
        ]);
      }
    }
  }
}
</script>

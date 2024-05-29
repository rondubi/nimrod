
enum action_t
{
        IN,
        OUT
};

// NOTE: should return status
// TODO: define what options are
int rule_add(int rule_number, enum action_t action, void * options);

// NOTE: should return status
// TODO: wtf is a packet_t
int rule_apply(packet_t packet);

// TODO: define what options are
int rule_config(int pipe_number, void * options);

// TODO: queue configuration



from time import sleep
from tether_agent.tether_agent import TetherAgent

def on_message(message):
    (topic, payload) = message
    print("*** Received from topic " + topic + ": " + str(payload))

agent = TetherAgent("test", "abc123")

in_plug = agent.create_input("test")
in_plug.add_listener(on_message)
out_plug = agent.create_output("test")

# Don't forget to connect!
agent.connect("127.0.0.1", 1883, "tether", "sp_ceB0ss!")

value = 0



while True:
    if agent.get_is_connected():
        sleep(1)
        out_plug.publish(None if value <= 0 else value)
        value += 1
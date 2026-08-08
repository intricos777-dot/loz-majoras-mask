#!/usr/bin/env python3
import subprocess, time, os, shutil

def run(cmd, timeout=10):
    r = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=timeout)
    print(f"$ {cmd}\n{r.stdout}{r.stderr}", end="")
    return r.returncode == 0

def main():
    home = os.path.expanduser("~")
    config = f"{home}/.config/plasma-org.kde.plasma.desktop-appletsrc"
    bak = f"{config}.bak.{time.strftime('%Y%m%d_%H%M%S')}"

    # backup
    if os.path.exists(config):
        shutil.copy(config, bak)
        print(f"Backup: {bak}")

    # stop plasmashell
    run("kquitapp5 plasmashell", timeout=5)
    run("killall plasmashell", timeout=3)
    time.sleep(2)

    # wipe panel config
    if os.path.exists(config):
        os.remove(config)

    # start plasmashell
    subprocess.Popen(["kstart5", "plasmashell"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    time.sleep(5)

    # use qdbus to configure panels
    # First check what's available
    run("qdbus org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell.activeLayout", timeout=5)

    # Try to add panels via D-Bus scripting
    script_top = """var panels = panelsByScreen(0);
var top = panelsByScreen(0).filter(function(p){ return p.location === "top"; })[0];
if (top) {
    top.widgets.forEach(function(w) { w.remove(); });
    var launcher = top.addWidget("org.kde.plasma.kickoff");
    launcher.alignment = "left";
    var tasks = top.addWidget("org.kde.plasma.taskmanager");
    tasks.alignment = "left";
    tasks.currentConfigGroup = ["General"];
    tasks.writeConfig("showOnlyCurrentScreen", true);
    var tray = top.addWidget("org.kde.plasma.systemtray");
    tray.alignment = "right";
    var clock = top.addWidget("org.kde.plasma.digitalclock");
    clock.alignment = "right";
    var showDesktop = top.addWidget("org.kde.plasma.showdesktop");
    showDesktop.alignment = "right";
}"""

    script_bottom = """var panels = panelsByScreen(0);
var bottom = panelsByScreen(0).filter(function(p){ return p.location === "bottom"; })[0];
if (!bottom) {
    bottom = new Panel;
    bottom.screen = 0;
    bottom.location = "bottom";
    bottom.height = 48;
}
if (bottom) {
    bottom.widgets.forEach(function(w) { w.remove(); });
    var launcher = bottom.addWidget("org.kde.plasma.kickoff");
    launcher.alignment = "left";
    var tasks = bottom.addWidget("org.kde.plasma.taskmanager");
    tasks.alignment = "left";
    tasks.currentConfigGroup = ["General"];
    tasks.writeConfig("showOnlyCurrentScreen", true);
    var showDesktop = bottom.addWidget("org.kde.plasma.showdesktop");
    showDesktop.alignment = "right";
}"""

    with open("/tmp/top_panel.js", "w") as f:
        f.write(script_top)
    with open("/tmp/bottom_dock.js", "w") as f:
        f.write(script_bottom)

    # Apply scripts via scripting interface
    run("qdbus org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell.evaluateScript \"" + script_top.replace("\n", " ").replace("\"", "\\\"") + "\"", timeout=5)
    time.sleep(1)
    run("qdbus org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell.evaluateScript \"" + script_bottom.replace("\n", " ").replace("\"", "\\\"") + "\"", timeout=5)
    time.sleep(2)

    # reload
    run("qdbus org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell.refreshCurrentShell", timeout=5)

    # restart plasmashell clean
    run("kquitapp5 plasmashell", timeout=5)
    time.sleep(2)
    subprocess.Popen(["kstart5", "plasmashell"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    time.sleep(4)

    print("=== Layout applied. Top bar + bottom dock. ===")

if __name__ == "__main__":
    main()

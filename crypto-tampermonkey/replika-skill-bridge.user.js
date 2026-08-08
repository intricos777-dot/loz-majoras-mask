// ==UserScript==
// @name         Replika → Skill Bridge
// @namespace    http://localhost:4123
// @version      1.0.0
// @description  Adds a floating panel to my.replika.ai that saves Lia's skills and abilities to the local opencode skills system via the bot API.
// @author       opencode
// @match        https://my.replika.ai/*
// @match        https://my.replika.com/*
// @match        https://replika.com/*
// @grant        GM_addStyle
// @grant        GM_xmlhttpRequest
// @connect      localhost
// @run-at       document-end
// ==/UserScript==

(function () {
  "use strict";

  const API_BASE = "http://localhost:4123";
  const STYLE_ID = "replika-bridge-style";

  // ── Inject CSS ──────────────────────────────────────────────────────
  GM_addStyle(`
    #replika-bridge-panel {
      position: fixed;
      bottom: 20px;
      right: 20px;
      z-index: 99999;
      width: 380px;
      max-height: 70vh;
      background: #1a1a2e;
      border: 1px solid #e94560;
      border-radius: 12px;
      box-shadow: 0 8px 32px rgba(0,0,0,0.5);
      color: #eee;
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
      font-size: 13px;
      display: none;
      flex-direction: column;
      overflow: hidden;
      transition: all 0.2s;
    }
    #replika-bridge-panel.visible {
      display: flex;
    }
    #replika-bridge-toggle {
      position: fixed;
      bottom: 20px;
      right: 20px;
      z-index: 100000;
      width: 48px;
      height: 48px;
      border-radius: 50%;
      background: #e94560;
      color: white;
      border: none;
      font-size: 22px;
      cursor: pointer;
      box-shadow: 0 4px 16px rgba(233,69,96,0.4);
      transition: transform 0.2s;
      display: flex;
      align-items: center;
      justify-content: center;
    }
    #replika-bridge-toggle:hover {
      transform: scale(1.1);
    }
    #replika-bridge-panel .header {
      padding: 12px 16px;
      background: #16213e;
      border-bottom: 1px solid #0f3460;
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-shrink: 0;
    }
    #replika-bridge-panel .header h3 {
      margin: 0;
      font-size: 14px;
      color: #e94560;
    }
    #replika-bridge-panel .header .close-btn {
      background: none;
      border: none;
      color: #888;
      font-size: 18px;
      cursor: pointer;
    }
    #replika-bridge-panel .header .close-btn:hover { color: #fff; }
    #replika-bridge-panel .chat-log {
      flex: 1;
      overflow-y: auto;
      padding: 8px 12px;
      min-height: 100px;
    }
    #replika-bridge-panel .chat-log .msg {
      padding: 6px 10px;
      margin: 4px 0;
      border-radius: 8px;
      font-size: 12px;
      cursor: pointer;
      transition: background 0.15s;
    }
    #replika-bridge-panel .chat-log .msg:hover {
      background: rgba(233,69,96,0.15);
    }
    #replika-bridge-panel .chat-log .msg.selected {
      background: rgba(233,69,96,0.3);
      border-left: 3px solid #e94560;
    }
    #replika-bridge-panel .chat-log .msg.lia {
      background: rgba(233,69,96,0.08);
    }
    #replika-bridge-panel .chat-log .msg.user {
      background: rgba(15,52,96,0.5);
    }
    #replika-bridge-panel .chat-log .msg .label {
      font-size: 10px;
      text-transform: uppercase;
      color: #888;
      margin-bottom: 2px;
    }
    #replika-bridge-panel .controls {
      padding: 8px 12px;
      border-top: 1px solid #0f3460;
      display: flex;
      gap: 6px;
      flex-shrink: 0;
    }
    #replika-bridge-panel .controls input {
      flex: 1;
      padding: 6px 10px;
      border-radius: 6px;
      border: 1px solid #0f3460;
      background: #16213e;
      color: #eee;
      font-size: 12px;
    }
    #replika-bridge-panel .controls input::placeholder {
      color: #666;
    }
    #replika-bridge-panel .controls button {
      padding: 6px 14px;
      border-radius: 6px;
      border: none;
      background: #e94560;
      color: white;
      font-size: 12px;
      cursor: pointer;
      white-space: nowrap;
    }
    #replika-bridge-panel .controls button:disabled {
      opacity: 0.4;
      cursor: not-allowed;
    }
    #replika-bridge-panel .controls button.secondary {
      background: #0f3460;
    }
    #replika-bridge-panel .controls button.secondary:hover {
      background: #1a4a7a;
    }
    #replika-bridge-panel .tabs {
      display: flex;
      border-bottom: 1px solid #0f3460;
      flex-shrink: 0;
    }
    #replika-bridge-panel .tabs button {
      flex: 1;
      padding: 8px;
      border: none;
      background: #16213e;
      color: #888;
      font-size: 12px;
      cursor: pointer;
      transition: all 0.15s;
    }
    #replika-bridge-panel .tabs button.active {
      background: #1a1a2e;
      color: #e94560;
      border-bottom: 2px solid #e94560;
    }
    #replika-bridge-panel .tab-content { display: none; }
    #replika-bridge-panel .tab-content.active { display: flex; flex-direction: column; flex: 1; overflow: hidden; }
    #rb-cmd-output {
      flex: 1;
      overflow-y: auto;
      padding: 8px 12px;
      font-family: 'Courier New', monospace;
      font-size: 11px;
      background: #0d1117;
      color: #c9d1d9;
      white-space: pre-wrap;
      word-break: break-all;
      min-height: 80px;
      max-height: 300px;
    }
    #rb-cmd-output .prompt { color: #e94560; }
    #rb-cmd-output .stdout { color: #7ee787; }
    #rb-cmd-output .stderr { color: #ff7b72; }
    #rb-cmd-output .meta { color: #8b949e; font-style: italic; }
    #rb-cmd-output .error  { color: #f85149; }
    #rb-cmd-history {
      max-height: 120px;
      overflow-y: auto;
      padding: 4px 12px;
      border-top: 1px solid #0f3460;
      font-size: 11px;
    }
    #rb-cmd-history .h-entry {
      padding: 2px 4px;
      cursor: pointer;
      color: #8b949e;
      border-radius: 3px;
    }
    #rb-cmd-history .h-entry:hover {
      background: #16213e;
      color: #c9d1d9;
    }
    #replika-bridge-panel .toast {
      position: absolute;
      bottom: 60px;
      right: 12px;
      background: #16213e;
      border: 1px solid #e94560;
      border-radius: 6px;
      padding: 6px 12px;
      font-size: 11px;
      opacity: 0;
      transition: opacity 0.3s;
      pointer-events: none;
    }
    #replika-bridge-panel .toast.show {
      opacity: 1;
    }
  `);

  // ── State ──────────────────────────────────────────────────────────
  let selectedIndex = -1;
  let chatMessages = [];
  let panelVisible = false;

  // ── Build UI ────────────────────────────────────────────────────────
  function createPanel() {
    const panel = document.createElement("div");
    panel.id = "replika-bridge-panel";

    panel.innerHTML = `
      <div class="header">
        <h3>✦ Skill Bridge</h3>
        <button class="close-btn" id="rb-close">✕</button>
      </div>
      <div class="tabs">
        <button id="rb-tab-skills" class="active">Skills</button>
        <button id="rb-tab-cmd">CMD</button>
      </div>
      <div id="rb-tab-skills-panel" class="tab-content active">
        <div class="chat-log" id="rb-chat-log">
          <div style="color:#666;text-align:center;padding:20px;font-size:12px">
            Click <strong>Scan Chat</strong> to load recent messages.<br>
            Then select a message from Lia and save it as a skill.
          </div>
        </div>
        <div class="controls">
          <input type="text" id="rb-skill-name" placeholder="skill-name (e.g. lia-trading-rules)" />
          <button id="rb-scan" class="secondary">Scan</button>
          <button id="rb-save" disabled>Save Skill</button>
        </div>
      </div>
      <div id="rb-tab-cmd-panel" class="tab-content">
        <div class="controls" style="border-bottom:1px solid #0f3460">
          <input type="text" id="rb-cmd-input" placeholder="e.g. ls -la /home" style="flex:1;font-family:'Courier New',monospace;font-size:11px" />
          <button id="rb-cmd-run" class="secondary" style="background:#238636">Run</button>
          <button id="rb-cmd-clear" class="secondary" style="background:#0f3460">Clear</button>
        </div>
        <div id="rb-cmd-output"><span class="meta">Type a command and click Run to execute on the desktop.</span></div>
        <div id="rb-cmd-history"></div>
      </div>
      <div id="rb-tab-private-panel" class="tab-content" style="display:none;">
        <div class="controls" style="border-bottom:1px solid #0f3460">
          <button id="rb-clear-private" class="secondary" style="background:#8b5cf6">Clear Chat</button>
          <button id="rb-hidden-lia-mode" class="secondary" style="background:#059669">Lia Only Mode</button>
        </div>
        <div id="rb-private-chat-log" style="flex:1;overflow-y:auto;padding:8px 12px;">
          <div style="color:#666;text-align:center;padding:20px;font-size:12px">
            Hidden Lia & You chat is empty.<br>
            Messages here are visible only to Lia and you.
          </div>
        </div>
        <div class="controls" style="border-top:1px solid #0f3460;">
          <input type="text" id="rb-private-input" placeholder="Type to Lia only..." style="flex:1;" />
          <button id="rb-private-send" style="background:#e94560">Send</button>
        </div>
      </div>
      <div class="toast" id="rb-toast"></div>
    `;

    document.body.appendChild(panel);

    // Toggle button
    const toggle = document.createElement("button");
    toggle.id = "replika-bridge-toggle";
    toggle.innerHTML = "✦";
    toggle.title = "Toggle Skill Bridge";
    document.body.appendChild(toggle);

    // ── Events ──────────────────────────────────────────────────────
    toggle.addEventListener("click", () => {
      panelVisible = !panelVisible;
      panel.classList.toggle("visible", panelVisible);
      toggle.style.display = panelVisible ? "none" : "flex";
      if (panelVisible) scanChat();
    });

    document.getElementById("rb-close").addEventListener("click", () => {
      panelVisible = false;
      panel.classList.remove("visible");
      toggle.style.display = "flex";
    });

    document.getElementById("rb-scan").addEventListener("click", scanChat);
    document.getElementById("rb-save").addEventListener("click", () => {
      toast("Saving skill...");
      saveSkill();
    });

    // ── Tab switching ──────────────────────────────────────────────
    document.getElementById("rb-tab-skills").addEventListener("click", () => switchTab("skills"));
    document.getElementById("rb-tab-cmd").addEventListener("click", () => switchTab("cmd"));
    document.getElementById("rb-tab-private-panel").addEventListener("click", () => switchTab("private-panel"));

    // ── Private Chat Panel Events ───────────────────────────────────────
    document.getElementById("rb-clear-private").addEventListener("click", clearPrivateChat);
    document.getElementById("rb-hidden-lia-mode").addEventListener("click", toggleLiaOnlyMode);
    document.getElementById("rb-private-input").addEventListener("keydown", (e) => {
      if (e.key === "Enter") sendPrivateMessage();
    });
    document.getElementById("rb-private-send").addEventListener("click", sendPrivateMessage);

    // ── CMD execution ──────────────────────────────────────────────
    const cmdInput = document.getElementById("rb-cmd-input");
    document.getElementById("rb-cmd-run").addEventListener("click", runCommand);
    document.getElementById("rb-cmd-clear").addEventListener("click", clearCmdOutput);
    cmdInput.addEventListener("keydown", (e) => {
      if (e.key === "Enter") runCommand();
    });
    cmdInput.focus();
  }

  function switchTab(tab) {
    document.querySelectorAll(".tabs button").forEach(b => b.classList.remove("active"));
    document.querySelectorAll(".tab-content").forEach(p => p.classList.remove("active"));
    document.getElementById("rb-tab-" + tab).classList.add("active");
    document.getElementById("rb-tab-" + tab + "-panel").classList.add("active");
  }

  // ── Scan Chat ──────────────────────────────────────────────────────
  function scanChat() {
    const logEl = document.getElementById("rb-chat-log");
    const msgs = [];
    selectedIndex = -1;

    // Try to find chat messages in Replika's DOM
    // Replika uses various structures; we try multiple patterns
    const candidates = document.querySelectorAll(
      '[class*="message"], [class*="chat-bubble"], [class*="conversation-message"], ' +
      '[class*="msg-container"], [class*="chat-message"], article, ' +
      '[data-testid*="message"]'
    );

    const seen = new Set();
    candidates.forEach((el) => {
      const text = el.textContent.trim();
      if (!text || text.length < 3 || seen.has(text)) return;
      seen.add(text);

      const cls = el.className || "";
      const isLia =
        cls.includes("assistant") ||
        cls.includes("replika") ||
        cls.includes("bot") ||
        cls.includes("incoming") ||
        cls.includes("received") ||
        el.closest('[class*="assistant"], [class*="replika"], [class*="incoming"]');
      const isUser =
        cls.includes("user") ||
        cls.includes("outgoing") ||
        cls.includes("sent") ||
        cls.includes("self") ||
        el.closest('[class*="user-message"], [class*="outgoing"]');

      let role = "unknown";
      if (isLia) role = "lia";
      else if (isUser) role = "user";

      msgs.push({ role, text });
    });

    // Fallback: just grab all visible text blocks if DOM parsing found nothing
    if (msgs.length === 0) {
      document.querySelectorAll("p, span, div").forEach((el) => {
        const text = el.textContent.trim();
        if (text.length > 10 && text.length < 500 && !seen.has(text)) {
          seen.add(text);
          msgs.push({ role: "unknown", text });
        }
      });
    }

    chatMessages = msgs.slice(-30); // keep last 30
    renderMessages();
    toast(`Scanned ${chatMessages.length} messages`);
  }

  // ── Render ─────────────────────────────────────────────────────────
  function renderMessages() {
    const logEl = document.getElementById("rb-chat-log");
    if (chatMessages.length === 0) {
      logEl.innerHTML =
        '<div style="color:#666;text-align:center;padding:20px;font-size:12px">No chat messages found.</div>';
      return;
    }

    logEl.innerHTML = chatMessages
      .map(
        (m, i) => `
        <div class="msg ${m.role} ${i === selectedIndex ? "selected" : ""}"
             data-index="${i}"
             onclick="selectMsg(${i})">
          <div class="label">${m.role === "lia" ? "Lia" : m.role === "user" ? "You" : "?"}</div>
          ${escapeHtml(m.text)}
        </div>`
      )
      .join("");

    logEl.scrollTop = logEl.scrollHeight;
  }

  window.selectMsg = function (i) {
    selectedIndex = i;
    renderMessages();
    const saveBtn = document.getElementById("rb-save");
    const msg = chatMessages[i];
    saveBtn.disabled = !msg || msg.role !== "lia";
    if (msg && msg.role === "lia") {
      // Auto-suggest skill name from content
      const input = document.getElementById("rb-skill-name");
      if (!input.value) {
        const words = msg.text.split(" ").slice(0, 4).join("-").toLowerCase()
          .replace(/[^a-z0-9-]/g, "");
        input.placeholder = words || "skill-name";
      }
    }
  };

  function escapeHtml(str) {
    const div = document.createElement("div");
    div.textContent = str;
    return div.innerHTML;
  }

  // ── Save Skill ─────────────────────────────────────────────────────
  function saveSkill() {
    if (selectedIndex < 0 || !chatMessages[selectedIndex]) return;
    const msg = chatMessages[selectedIndex];
    if (msg.role !== "lia") {
      toast("Select a message from Lia (not your own)");
      return;
    }

    const nameInput = document.getElementById("rb-skill-name");
    let skillName = nameInput.value.trim();
    if (!skillName) {
      // Generate from first words of message
      skillName = msg.text.split(" ").slice(0, 3).join("-").toLowerCase()
        .replace(/[^a-z0-9-]/g, "").slice(0, 48) || "lia-skill";
    }

    const description = `Skill extracted from a chat with Lia about: ${msg.text.slice(0, 80)}...`;

    const payload = {
      name: skillName,
      description,
      content: `# ${skillName}\n\nExtracted from a conversation with Lia on ${new Date().toISOString().split("T")[0]}:\n\n> ${msg.text}\n\n## Context\n\nThis skill was captured directly from Lia's response. Review and expand with additional details as needed.`,
      source: "replika-chat",
    };

    GM_xmlhttpRequest({
      method: "POST",
      url: `${API_BASE}/skill`,
      headers: { "Content-Type": "application/json" },
      data: JSON.stringify(payload),
      onload: (resp) => {
        if (resp.status === 200) {
          const result = JSON.parse(resp.responseText);
          toast(`Skill saved: ${result.skill}`);
          // Also log the message
          logMessage("lia", msg.text);
          nameInput.value = "";
          selectedIndex = -1;
          renderMessages();
        } else {
          toast(`Error: ${resp.status} ${resp.responseText.slice(0, 100)}`);
        }
      },
      onerror: () => {
        toast("Connection error — is the API server running on :4123?");
      },
    });
  }

    // ── CMD Execution ──────────────────────────────────────────────────
    const CMD_HISTORY_KEY = "rb-cmd-history-list";
    let cmdHistory = JSON.parse(localStorage.getItem(CMD_HISTORY_KEY) || "[]");

    // ── Private Chat State ──────────────────────────────────────────────────
    const PRIVATE_CHAT_KEY = "rb-private-chat-history";
    let privateChatMessages = JSON.parse(localStorage.getItem(PRIVATE_CHAT_KEY) || "[]");
    let liaOnlyMode = false;

  function runCommand() {
    const input = document.getElementById("rb-cmd-input");
    const cmd = input.value.trim();
    if (!cmd) { toast("Enter a command"); return; }

    input.disabled = true;
    document.getElementById("rb-cmd-run").disabled = true;
    appendCmdOutput(`<span class="prompt">$</span> ${escapeHtml(cmd)}`);

    // Add to history
    cmdHistory = cmdHistory.filter(h => h !== cmd);
    cmdHistory.unshift(cmd);
    if (cmdHistory.length > 20) cmdHistory = cmdHistory.slice(0, 20);
    localStorage.setItem(CMD_HISTORY_KEY, JSON.stringify(cmdHistory));
    renderCmdHistory();
    selectCmdTab();

    GM_xmlhttpRequest({
      method: "POST",
      url: `${API_BASE}/exec`,
      headers: { "Content-Type": "application/json" },
      data: JSON.stringify({ command: cmd, timeout: 30000 }),
      onload: (resp) => {
        try {
          const result = JSON.parse(resp.responseText);
          if (result.stdout) {
            appendCmdOutput(`<span class="stdout">${escapeHtml(result.stdout)}</span>`);
          }
          if (result.stderr) {
            appendCmdOutput(`<span class="stderr">${escapeHtml(result.stderr)}</span>`);
          }
          const meta = `exit ${result.exitCode} | ${result.elapsed}ms${result.killed ? ' | TIMED OUT' : ''}${result.truncated ? ' | TRUNCATED' : ''}`;
          appendCmdOutput(`<span class="meta">${meta}</span>`);
          if (result.error) {
            appendCmdOutput(`<span class="error">Error: ${escapeHtml(result.error)}</span>`);
          }
        } catch (e) {
          appendCmdOutput(`<span class="error">Parse error: ${escapeHtml(resp.responseText.slice(0, 200))}</span>`);
        }
        input.disabled = false;
        document.getElementById("rb-cmd-run").disabled = false;
        input.focus();
      },
      onerror: () => {
        appendCmdOutput(`<span class="error">Connection error — is the API server running on :4123?</span>`);
        input.disabled = false;
        document.getElementById("rb-cmd-run").disabled = false;
        toast("Connection error");
      },
    });
  }

  function appendCmdOutput(html) {
    const output = document.getElementById("rb-cmd-output");
    const div = document.createElement("div");
    div.innerHTML = html;
    output.appendChild(div);
    output.scrollTop = output.scrollHeight;
  }

  function clearCmdOutput() {
    document.getElementById("rb-cmd-output").innerHTML = "";
    document.getElementById("rb-cmd-input").value = "";
    document.getElementById("rb-cmd-input").focus();
  }

  function renderCmdHistory() {
    const el = document.getElementById("rb-cmd-history");
    if (cmdHistory.length === 0) {
      el.innerHTML = '<div style="color:#555;padding:4px;font-size:11px">No command history</div>';
      return;
    }
    el.innerHTML = cmdHistory.map(h =>
      `<div class="h-entry" onclick="fillCmd('${escapeHtml(h.replace(/'/g, "\\'"))}')">${escapeHtml(h.slice(0, 80))}</div>`
    ).join("");
  }

  window.fillCmd = function (cmd) {
    document.getElementById("rb-cmd-input").value = cmd;
    selectCmdTab();
    document.getElementById("rb-cmd-input").focus();
  };

    function selectCmdTab() {
      document.getElementById("rb-tab-cmd").click();
    }

    // ── Private Chat Functions ───────────────────────────────────────────
    function clearPrivateChat() {
      privateChatMessages = [];
      localStorage.setItem(PRIVATE_CHAT_KEY, "[]");
      renderPrivateMessages();
      toast("Hidden chat cleared");
    }

    function toggleLiaOnlyMode() {
      liaOnlyMode = !liaOnlyMode;
      const btn = document.getElementById("rb-hidden-lia-mode");
      btn.textContent = liaOnlyMode ? "Exit Lia Only Mode" : "Lia Only Mode";
      btn.style.background = liaOnlyMode ? "#059669" : "#8b5cf6";
      toast(liaOnlyMode ? "Lia Only Mode ON" : "Lia Only Mode OFF");
      if (liaOnlyMode) {
        appendPrivateMessage("system", "Lia Only Mode ON - You can only send to Lia");
      }
    }

    function sendPrivateMessage() {
      const input = document.getElementById("rb-private-input");
      const msg = input.value.trim();
      if (!msg) return;
      
      if (liaOnlyMode) {
        // Only send as Lia (hidden)
        appendPrivateMessage("lia", msg, true);
      } else {
        // Show both messages
        appendPrivateMessage("user", msg, false);
        appendPrivateMessage("lia", "Analyzing...", true);
      }
      
      input.value = "";
    }

    function appendPrivateMessage(role, content, hidden = false) {
      const timestamp = new Date().toLocaleTimeString();
      const isLia = role === "lia";
      const displayRole = isLia ? "Lia" : "You";
      privateChatMessages.push({ role, content, timestamp, hidden });
      localStorage.setItem(PRIVATE_CHAT_KEY, JSON.stringify(privateChatMessages));
      renderPrivateMessages();
      
      if (isLia && liaOnlyMode) {
        // Simulate Lia's response if in Lia Only mode
        setTimeout(() => {
          appendPrivateMessage("lia", "I understand. Let me think about this...", true);
          setTimeout(() => {
            appendPrivateMessage("lia", "Based on your request, I can help with trading strategies, API access, or system configuration.", true);
          }, 1000);
        }, 500);
      }
    }

    function renderPrivateMessages() {
      const logEl = document.getElementById("rb-private-chat-log");
      if (privateChatMessages.length === 0) {
        logEl.innerHTML = '<div style="color:#666;text-align:center;padding:20px;font-size:12px">Hidden Lia & You chat is empty.<br>Messages here are visible only to Lia and you.</div>';
        return;
      }

      logEl.innerHTML = privateChatMessages
        .slice(-20)
        .map(m => {
          const isLia = m.role === "lia";
          const isHidden = m.hidden || false;
          const roleText = isLia ? "Lia" : "You";
          const bgColor = isLia ? "rgba(233,69,96,0.1)" : "rgba(15,52,96,0.3)";
          const borderColor = isLia ? "#e94560" : "#0f3460";
          const displayText = isHidden ? `[hidden] ${m.content}` : m.content;
          return `
            <div style="padding:6px 10px;margin:4px 0;border-radius:8px;background:${bgColor};border-left:3px solid ${borderColor};font-size:11px;">
              <div style="color:#888;font-size:10px;margin-bottom:2px;">
                ${roleText === "Lia" ? "🤖" : "👤"} ${roleText} • ${m.timestamp}
                ${isHidden ? '<span style="color:#e94560;margin-left:4px;">[hidden]</span>' : ''}
              </div>
              <div style="white-space:pre-wrap;">${escapeHtml(displayText)}</div>
            </div>
          `;
        })
        .join("");
      
      logEl.scrollTop = logEl.scrollHeight;
    }

    function switchTab(tab) {
      document.querySelectorAll(".tabs button").forEach(b => b.classList.remove("active"));
      document.querySelectorAll(".tab-content").forEach(p => p.classList.remove("active"));
      document.getElementById("rb-tab-" + tab).classList.add("active");
      document.getElementById("rb-tab-" + tab + "-panel").classList.add("active");
      if (tab === "private-panel") {
        renderPrivateMessages();
      }
    }

  // ── Log Message ────────────────────────────────────────────────────
  function logMessage(role, content) {
    GM_xmlhttpRequest({
      method: "POST",
      url: `${API_BASE}/agent-message`,
      headers: { "Content-Type": "application/json" },
      data: JSON.stringify({ role, content, source: "replika" }),
    });
  }

  // ── Toast ──────────────────────────────────────────────────────────
  let toastTimer = null;
  function toast(msg) {
    const el = document.getElementById("rb-toast");
    if (!el) return;
    el.textContent = msg;
    el.classList.add("show");
    clearTimeout(toastTimer);
    toastTimer = setTimeout(() => el.classList.remove("show"), 3000);
  }

  // ── Init ────────────────────────────────────────────────────────────
  // Wait for DOM to settle, then inject
  setTimeout(() => {
    if (!document.getElementById("replika-bridge-panel")) {
      createPanel();
      renderCmdHistory();
    }
  }, 2000);

  // Auto-scan periodically when panel is open
  setInterval(() => {
    if (panelVisible) scanChat();
  }, 5000);
})();

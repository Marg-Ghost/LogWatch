const systemInfo = document.getElementById("system-info");
const loadStatus = document.getElementById("load-status");

function create_card(title) {
    const card = document.createElement("article");
    card.className = "system-card";
    const heading = document.createElement("h2");
    heading.textContent = title;
    card.appendChild(heading);
    return card;
}

function add_value(card, label, value) {
    const row = document.createElement("div");
    row.className = "system-value";
    const labelText = document.createElement("span");
    labelText.textContent = label;
    const valueText = document.createElement("span");
    valueText.textContent = value;
    row.append(labelText, valueText);
    card.appendChild(row);
}

function add_meter(card, label, percentage, detail) {
    const meter = document.createElement("div");
    meter.className = "system-meter";
    const meterLabel = document.createElement("div");
    meterLabel.className = "system-meter-label";
    const labelText = document.createElement("span");
    labelText.textContent = label;
    const detailText = document.createElement("span");
    detailText.textContent = detail;
    meterLabel.append(labelText, detailText);

    const progress = document.createElement("progress");
    progress.max = 100;
    progress.value = Math.max(0, Math.min(100, percentage));
    progress.setAttribute("aria-label", label);
    meter.append(meterLabel, progress);
    card.appendChild(meter);
}

function render_system_info(data) {
    systemInfo.replaceChildren();

    const account = create_card("System");
    add_value(account, "User", data.usr);
    add_value(account, "Additional info", data.fastf.length > 0 ? data.fastf.join(", ") : "No additional data");
    systemInfo.appendChild(account);

    const dateTime = create_card("Date and time");
    add_value(dateTime, "Date", data.timedate.date);
    add_value(dateTime, "Time", data.timedate.time);
    systemInfo.appendChild(dateTime);

    const cpu = create_card("CPU");
    add_value(cpu, "Temperature", `${data.cpu_temp.toFixed(1)} °C`);
    data.cores.forEach((percentage, index) => {
        add_meter(cpu, `Core ${index + 1}`, percentage, `${percentage.toFixed(1)}%`);
    });
    systemInfo.appendChild(cpu);

    const ram = create_card("Memory");
    add_meter(
        ram,
        "RAM",
        data.ram.used_percent,
        `${data.ram.used} / ${data.ram.total} GiB (${data.ram.used_percent}%)`
    );
    systemInfo.appendChild(ram);

    const disk = create_card("Disk");
    const used = Math.max(0, data.disk.total - data.disk.free);
    add_meter(
        disk,
        "Usage",
        data.disk.used_percent,
        `${used.toFixed(1)} / ${data.disk.total} GiB (${data.disk.used_percent}%)`
    );
    add_value(disk, "Free", `${data.disk.free} GiB`);
    systemInfo.appendChild(disk);

    const network = create_card("Network");
    add_value(network, "Ethernet", `${data.network.ethernet.name} - ${data.network.ethernet.status}`);
    add_value(network, "Wi-Fi", `${data.network.wifi.name} - ${data.network.wifi.status}`);
    systemInfo.appendChild(network);
}

async function load_sys_info() {
    loadStatus.textContent = "Loading...";
    try {
        const response = await fetch("/load_sys_info");
        if (!response.ok) {
            throw new Error(`Request failed (${response.status})`);
        }
        const data = await response.json();
        render_system_info(data);
        loadStatus.textContent = "System information loaded";
    } catch (error) {
        systemInfo.replaceChildren();
        const message = document.createElement("p");
        message.className = "state-message error";
        message.textContent = `Could not load system information: ${error.message}`;
        systemInfo.appendChild(message);
        loadStatus.textContent = "Load failed";
    }
}

document.getElementById("reload-system").addEventListener("click", load_sys_info);

load_sys_info();

const containerList = document.getElementById("main");
const loadStatus = document.getElementById("load-status");
const detailsPanel = document.getElementById("container-details");

let containers = [];
let selectedContainer = null;
let selectedErrors = [];
let selectedErrorIndex = 0;

async function load_all_server(){
    loadStatus.textContent = "Loading...";
    try {
        const response = await fetch("/load_all_servers");
        if (!response.ok) {
            throw new Error(`Request failed (${response.status})`);
        }
        const data = await response.json();
        if (!data || !Array.isArray(data.data)) {
            throw new Error("The server returned an invalid container list");
        }

        containers = data.data;
        containerList.replaceChildren();
        if (containers.length === 0) {
            const message = document.createElement("p");
            message.className = "state-message";
            message.textContent = "No Docker-Container found.";
            containerList.appendChild(message);
        }

        for (const container of containers) {
            //takes the dicts in the list and formates them into html div elements in id = main
            //also every entrie needs a button to call /server_status/basic/reqeust_type_1 to call if
            //a seerver needs checkup
            const problemCount = get_problem_count(container);
            const card = document.createElement("article");
            card.className = `container-card${problemCount > 0 ? " has-problems" : ""}`;

            const title = document.createElement("button");
            title.className = "container-title";
            title.type = "button";
            title.textContent = container.name || `Container ${container.id}`;
            title.addEventListener("click", () => show_container_details(container));

            const meta = document.createElement("div");
            meta.className = "container-meta";
            const lastUpdated = document.createElement("p");
            lastUpdated.textContent = `last updated: ${format_date(container.last_updated)}`;
            const status = document.createElement("p");
            status.textContent = `status: ${container.status || "unknown"}`;
            const type = document.createElement("p");
            type.textContent = `type: ${container.type || "unknown"}`;
            const version = document.createElement("p");
            version.textContent = `version: ${container.version || "unknown"}`;
            const importance = document.createElement("p");
            const importanceValue = container.importance !== undefined && container.importance !== null ?
                container.importance : "unknown";
            importance.textContent = `importance: ${importanceValue}`;
            meta.append(lastUpdated, status, type, version, importance);

            const count = document.createElement("p");
            count.className = "problem-count";
            count.textContent = `${problemCount}-Problems`;

            card.append(title, meta, count);
            if (problemCount > 0) {
                const indicator = document.createElement("span");
                indicator.className = "status-indicator";
                indicator.setAttribute("role", "img");
                indicator.setAttribute("aria-label", `${problemCount} problems`);
                card.appendChild(indicator);
            }
            containerList.appendChild(card);
        }

        loadStatus.textContent = `${containers.length} containers loaded`;
    } catch (error) {
        containerList.replaceChildren();
        const message = document.createElement("p");
        message.className = "state-message error";
        message.textContent = `Could not load containers: ${error.message}`;
        containerList.appendChild(message);
        loadStatus.textContent = "Load failed";
    }
}

function get_problem_count(container) {
    if (typeof container.problems === "number" && Number.isFinite(container.problems)) {
        return container.problems;
    }
    if (typeof container.error_count === "number" && Number.isFinite(container.error_count)) {
        return container.error_count;
    }
    if (Array.isArray(container.errors)) {
        return container.errors.length;
    }
    if (container.errors && typeof container.errors === "object") {
        return Object.keys(container.errors).length;
    }
    return 0;
}

function format_date(value) {
    if (!value) {
        return "Unknown";
    }
    const date = new Date(value);
    return Number.isNaN(date.getTime()) ? String(value) : date.toLocaleString();
}

function show_container_details(container) {
    selectedContainer = container;
    selectedErrorIndex = 0;
    if (Array.isArray(container.errors)) {
        selectedErrors = container.errors.map((error, index) => [index + 1, error]);
    } else if (container.errors && typeof container.errors === "object") {
        selectedErrors = Object.entries(container.errors);
    } else if (container.errors) {
        selectedErrors = [[1, container.errors]];
    } else {
        selectedErrors = [];
    }

    document.getElementById("detail-title").textContent = container.name || `Container ${container.id}`;
    render_selected_error();
    detailsPanel.hidden = false;
}

function render_selected_error() {
    const message = document.getElementById("error-message");
    const counter = document.getElementById("error-counter");
    const previous = document.getElementById("previous-error");
    const next = document.getElementById("next-error");

    if (selectedErrors.length === 0) {
        message.textContent = "No errors recorded.";
        counter.textContent = "0/0";
    } else {
        const [number, error] = selectedErrors[selectedErrorIndex];
        const errorText = typeof error === "string" ? error : JSON.stringify(error);
        message.textContent = `Error ${number}: ${errorText}`;
        counter.textContent = `${selectedErrorIndex + 1}/${selectedErrors.length}`;
    }

    previous.disabled = selectedErrorIndex === 0;
    next.disabled = selectedErrors.length === 0 || selectedErrorIndex === selectedErrors.length - 1;
    document.getElementById("solution-text").textContent =
        selectedContainer.solution || selectedContainer.ai_answer || "No AI answer available.";
}

document.getElementById("reload").addEventListener("click", load_all_server);
document.getElementById("close-details").addEventListener("click", () => {
    detailsPanel.hidden = true;
});
document.getElementById("previous-error").addEventListener("click", () => {
    selectedErrorIndex -= 1;
    render_selected_error();
});
document.getElementById("next-error").addEventListener("click", () => {
    selectedErrorIndex += 1;
    render_selected_error();
});
load_all_server();

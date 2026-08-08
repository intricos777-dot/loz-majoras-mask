#!/usr/bin/env python3
"""
AI-Integrated Resource Allocation Optimizer
Dynamically optimizes CPU/GPU/memory allocation for mining workloads
using predictive modeling and real-time telemetry.
"""

import os
import sys
import json
import time
import subprocess
import threading
import psutil
import numpy as np
from collections import deque
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Tuple, Optional
import logging

# ─── Configuration ──────────────────────────────────────────────
CONFIG = {
    "optimization_interval": 30,  # seconds
    "telemetry_window": 300,      # 5 min rolling window
    "cpu": {
        "p_cores": [0, 1, 2, 3],        # Performance cores
        "e_cores": [4, 5, 6, 7],        # Efficiency cores
        "p_threads": [0, 1, 2, 3, 4, 5, 6, 7],  # P-core threads
        "e_threads": [8, 9, 10, 11],    # E-core threads
        "mining_affinity": "p_cores",   # or "e_cores", "hybrid"
        "nice_mining": 10,
        "nice_system": 0,
        "nice_ui": -5,
    },
    "gpu": {
        "memory_limit_mb": 3500,        # Reserve 500MB for display
        "compute_mode": "exclusive",
        "power_limit_w": 55,
        "temp_limit_c": 75,
    },
    "memory": {
        "swapiness": 10,
        "vfs_cache_pressure": 50,
        "min_free_kb": 524288,          # 512MB
        "compact_memory": True,
    },
    "miners": {
        "btc_solo": {
            "service": "btc-solo-miner",
            "cpu_threads": 4,
            "prefer_p_cores": True,
            "memory_mb": 50,
        },
        "rvn_gpu": {
            "services": ["gpu-miner", "rvn-miner"],
            "gpu_memory_mb": 74,
            "gpu_intensity": 26,
        },
        "ltc_cpu": {
            "pattern": "ltc.viabtc.io",
            "cpu_threads": 6,
            "prefer_e_cores": True,
        },
        "doge_cpu": {
            "pattern": "zpool.ca:3433",
            "cpu_threads": 6,
            "prefer_e_cores": True,
        },
    },
    "ai": {
        "enabled": True,
        "model_path": "/tmp/ai_resource_model.pkl",
        "retrain_interval": 3600,       # 1 hour
        "features": ["cpu_p_util", "cpu_e_util", "gpu_util", "gpu_mem", "mem_pressure", "swap_used", "load_avg"],
        "target": "optimal_throughput",
    }
}

# ─── Logging ────────────────────────────────────────────────────
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s [%(levelname)s] %(message)s',
    handlers=[
        logging.FileHandler('/var/log/ai-resource-optimizer.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

# ─── AI Model (Simple Predictive) ───────────────────────────────
class ResourcePredictor:
    """Lightweight predictive model for resource optimization."""
    
    def __init__(self, config: Dict):
        self.config = config
        self.history = deque(maxlen=config["telemetry_window"])
        self.weights = np.random.randn(len(config["ai"]["features"])) * 0.1
        self.bias = 0.0
        self.learning_rate = 0.01
        self.trained = False
        
    def extract_features(self, telemetry: Dict) -> np.ndarray:
        """Extract feature vector from telemetry."""
        features = []
        features.append(telemetry.get("cpu_p_util", 0))
        features.append(telemetry.get("cpu_e_util", 0))
        features.append(telemetry.get("gpu_util", 0))
        features.append(telemetry.get("gpu_mem_pct", 0))
        features.append(telemetry.get("mem_pressure", 0))
        features.append(telemetry.get("swap_used_pct", 0))
        features.append(telemetry.get("load_avg", 0))
        return np.array(features)
    
    def predict(self, telemetry: Dict) -> Dict:
        """Predict optimal resource allocation."""
        if not self.trained:
            return self._heuristic_allocation(telemetry)
        
        x = self.extract_features(telemetry)
        score = np.dot(x, self.weights) + self.bias
        
        # Map score to allocation decisions
        return {
            "mining_cpu_affinity": "p_cores" if score > 0 else "e_cores",
            "gpu_intensity": min(30, max(20, int(26 + score * 4))),
            "nice_adjustment": int(-score * 2),
            "memory_pressure_action": "compact" if telemetry.get("mem_pressure", 0) > 0.8 else "none",
        }
    
    def _heuristic_allocation(self, telemetry: Dict) -> Dict:
        """Fallback heuristic allocation."""
        cpu_p = telemetry.get("cpu_p_util", 0)
        cpu_e = telemetry.get("cpu_e_util", 0)
        gpu_mem = telemetry.get("gpu_mem_pct", 0)
        mem_pressure = telemetry.get("mem_pressure", 0)
        
        # Prefer P-cores for mining if available, else E-cores
        mining_affinity = "p_cores" if cpu_p < 0.7 else "e_cores"
        
        # GPU intensity based on memory pressure
        gpu_intensity = 26
        if gpu_mem > 0.85:
            gpu_intensity = 20
        elif gpu_mem < 0.6:
            gpu_intensity = 28
            
        return {
            "mining_cpu_affinity": mining_affinity,
            "gpu_intensity": gpu_intensity,
            "nice_adjustment": 0,
            "memory_pressure_action": "compact" if mem_pressure > 0.85 else "none",
        }
    
    def train(self, telemetry: Dict, reward: float):
        """Online learning from observed rewards."""
        if not self.config["ai"]["enabled"]:
            return
            
        x = self.extract_features(telemetry)
        prediction = np.dot(x, self.weights) + self.bias
        error = reward - prediction
        
        # Gradient descent update
        self.weights += self.learning_rate * error * x
        self.bias += self.learning_rate * error
        self.trained = True
        
        # Store for batch retraining
        self.history.append((telemetry, reward))

# ─── System Telemetry ──────────────────────────────────────────
class SystemTelemetry:
    """Collects real-time system metrics."""
    
    def __init__(self):
        self.gpu_available = self._check_gpu()
        
    def _check_gpu(self) -> bool:
        try:
            subprocess.run(["nvidia-smi"], capture_output=True, check=True)
            return True
        except:
            return False
    
    def collect(self) -> Dict:
        """Collect all telemetry metrics."""
        cpu_times = psutil.cpu_times_percent(percpu=True, interval=0.1)
        cpu_percent = psutil.cpu_percent(percpu=True, interval=0.1)
        
        # Separate P-core and E-core utilization
        p_core_util = np.mean([cpu_percent[i] for i in CONFIG["cpu"]["p_threads"] if i < len(cpu_percent)])
        e_core_util = np.mean([cpu_percent[i] for i in CONFIG["cpu"]["e_threads"] if i < len(cpu_percent)])
        
        mem = psutil.virtual_memory()
        swap = psutil.swap_memory()
        load = psutil.getloadavg()[0] / psutil.cpu_count()
        
        telemetry = {
            "timestamp": time.time(),
            "cpu_p_util": p_core_util / 100.0,
            "cpu_e_util": e_core_util / 100.0,
            "cpu_overall": np.mean(cpu_percent) / 100.0,
            "mem_used_pct": mem.percent / 100.0,
            "mem_available_gb": mem.available / (1024**3),
            "swap_used_pct": swap.percent / 100.0,
            "load_avg": load,
            "mem_pressure": 1.0 - (mem.available / mem.total),
        }
        
        if self.gpu_available:
            telemetry.update(self._get_gpu_telemetry())
            
        return telemetry
    
    def _get_gpu_telemetry(self) -> Dict:
        """Get GPU metrics via nvidia-smi."""
        try:
            out = subprocess.check_output([
                "nvidia-smi", "--query-gpu=utilization.gpu,memory.used,memory.total,temperature.gpu,power.draw",
                "--format=csv,noheader,nounits"
            ], text=True, timeout=2).strip()
            
            util, mem_used, mem_total, temp, power = map(float, out.split(", "))
            return {
                "gpu_util": util / 100.0,
                "gpu_mem_used_mb": mem_used,
                "gpu_mem_total_mb": mem_total,
                "gpu_mem_pct": mem_used / mem_total,
                "gpu_temp_c": temp,
                "gpu_power_w": power,
            }
        except:
            return {
                "gpu_util": 0, "gpu_mem_used_mb": 0, "gpu_mem_total_mb": 4096,
                "gpu_mem_pct": 0, "gpu_temp_c": 0, "gpu_power_w": 0
            }

# ─── Resource Controller ────────────────────────────────────────
class ResourceController:
    """Applies resource allocation decisions."""
    
    def __init__(self, config: Dict):
        self.config = config
        self.current_affinity = {}
        self.applied_intensity = {}
        
    def apply_allocation(self, allocation: Dict, telemetry: Dict):
        """Apply resource allocation decisions."""
        self._apply_cpu_affinity(allocation.get("mining_cpu_affinity", "p_cores"))
        self._apply_gpu_settings(allocation.get("gpu_intensity", 26))
        self._apply_nice_values(allocation.get("nice_adjustment", 0))
        self._apply_memory_actions(allocation.get("memory_pressure_action", "none"))
        self._optimize_miner_processes(telemetry)
        
    def _apply_cpu_affinity(self, affinity: str):
        """Set CPU affinity for mining processes."""
        if affinity == "p_cores":
            mask = self._cores_to_mask(self.config["cpu"]["p_threads"])
        elif affinity == "e_cores":
            mask = self._cores_to_mask(self.config["cpu"]["e_threads"])
        else:  # hybrid
            mask = self._cores_to_mask(self.config["cpu"]["p_threads"] + self.config["cpu"]["e_threads"])
            
        # Apply to mining processes
        for miner_name, miner_config in self.config["miners"].items():
            if miner_name == "btc_solo":
                self._set_process_affinity(miner_config["service"], mask)
            elif "pattern" in miner_config:
                self._set_pattern_affinity(miner_config["pattern"], mask)
                
    def _cores_to_mask(self, cores: List[int]) -> str:
        """Convert core list to hex affinity mask."""
        mask = 0
        for c in cores:
            mask |= (1 << c)
        return hex(mask)
    
    def _set_process_affinity(self, service: str, mask: str):
        """Set CPU affinity for systemd service."""
        try:
            # Get main PID
            out = subprocess.check_output(
                ["systemctl", "show", service, "--property=MainPID"],
                text=True, timeout=2
            ).strip()
            pid = int(out.split("=")[1])
            if pid > 0:
                subprocess.run(["taskset", "-p", mask, str(pid)], capture_output=True)
                logger.debug(f"Set {service} (PID {pid}) affinity to {mask}")
        except Exception as e:
            logger.debug(f"Failed to set affinity for {service}: {e}")
    
    def _set_pattern_affinity(self, pattern: str, mask: str):
        """Set affinity for processes matching pattern."""
        try:
            out = subprocess.check_output(
                ["pgrep", "-f", pattern], text=True, timeout=2
            ).strip()
            for pid in out.split("\n"):
                if pid:
                    subprocess.run(["taskset", "-p", mask, pid], capture_output=True)
        except Exception as e:
            logger.debug(f"Failed to set pattern affinity for {pattern}: {e}")
    
    def _apply_gpu_settings(self, intensity: int):
        """Apply GPU compute settings."""
        try:
            # Set power limit
            subprocess.run([
                "nvidia-smi", "-pl", str(self.config["gpu"]["power_limit_w"])
            ], capture_output=True, timeout=5)
            
            # Set compute mode
            subprocess.run([
                "nvidia-smi", "-c", self.config["gpu"]["compute_mode"]
            ], capture_output=True, timeout=5)
            
            logger.debug(f"Applied GPU settings: power={self.config['gpu']['power_limit_w']}W, intensity={intensity}")
        except Exception as e:
            logger.debug(f"GPU settings failed: {e}")
    
    def _apply_nice_values(self, adjustment: int):
        """Adjust process priorities."""
        nice_mining = self.config["cpu"]["nice_mining"] + adjustment
        try:
            for miner_name, miner_config in self.config["miners"].items():
                if "service" in miner_config:
                    self._set_service_nice(miner_config["service"], nice_mining)
        except Exception as e:
            logger.debug(f"Nice adjustment failed: {e}")
    
    def _set_service_nice(self, service: str, nice: int):
        """Set nice value for service."""
        try:
            out = subprocess.check_output(
                ["systemctl", "show", service, "--property=MainPID"],
                text=True, timeout=2
            ).strip()
            pid = int(out.split("=")[1])
            if pid > 0:
                os.nice(nice - os.getpriority(os.PRIO_PROCESS, pid))
        except:
            pass
    
    def _apply_memory_actions(self, action: str):
        """Apply memory management actions."""
        if action == "compact":
            try:
                with open("/proc/sys/vm/compact_memory", "w") as f:
                    f.write("1")
                logger.info("Memory compaction triggered")
            except:
                pass
    
    def _optimize_miner_processes(self, telemetry: Dict):
        """Fine-tune individual miner processes."""
        gpu_mem_pct = telemetry.get("gpu_mem_pct", 0)
        mem_pressure = telemetry.get("mem_pressure", 0)
        
        # Adjust RVN miner intensity based on GPU memory
        if gpu_mem_pct > 0.85:
            self._adjust_srbminer_intensity(20)
        elif gpu_mem_pct < 0.6:
            self._adjust_srbminer_intensity(28)
            
        # Kill/restart memory-hungry faucet browser if pressure high
        if mem_pressure > 0.9:
            self._restart_faucet_daemon()
    
    def _adjust_srbminer_intensity(self, intensity: int):
        """Adjust SRBMiner intensity via API or restart."""
        # Would need API support or service restart
        pass
    
    def _restart_faucet_daemon(self):
        """Restart faucet daemon to free memory."""
        try:
            subprocess.run(["systemctl", "restart", "faucet-daemon"], timeout=30)
            logger.warning("Restarted faucet-daemon due to memory pressure")
        except:
            pass

# ─── Main Optimizer ────────────────────────────────────────────
class AIResourceOptimizer:
    """Main optimization loop."""
    
    def __init__(self, config: Dict):
        self.config = config
        self.telemetry = SystemTelemetry()
        self.predictor = ResourcePredictor(config)
        self.controller = ResourceController(config)
        self.running = False
        self.step = 0
        
    def start(self):
        """Start optimization loop."""
        self.running = True
        logger.info("AI Resource Optimizer started")
        
        # Apply initial system tuning
        self._apply_system_tuning()
        
        while self.running:
            try:
                self._optimization_step()
            except Exception as e:
                logger.error(f"Optimization step failed: {e}")
            time.sleep(self.config["optimization_interval"])
    
    def stop(self):
        """Stop optimization loop."""
        self.running = False
        logger.info("AI Resource Optimizer stopped")
    
    def _apply_system_tuning(self):
        """Apply persistent system-level optimizations."""
        try:
            # VM settings
            with open("/proc/sys/vm/swappiness", "w") as f:
                f.write(str(self.config["memory"]["swappiness"]))
            with open("/proc/sys/vm/vfs_cache_pressure", "w") as f:
                f.write(str(self.config["memory"]["vfs_cache_pressure"]))
            with open("/proc/sys/vm/min_free_kbytes", "w") as f:
                f.write(str(self.config["memory"]["min_free_kb"]))
            
            # CPU governor
            for cpu in range(psutil.cpu_count()):
                try:
                    with open(f"/sys/devices/system/cpu/cpu{cpu}/cpufreq/scaling_governor", "w") as f:
                        f.write("performance")
                except:
                    pass
                    
            logger.info("System tuning applied")
        except Exception as e:
            logger.warning(f"System tuning partial: {e}")
    
    def _optimization_step(self):
        """Single optimization iteration."""
        # Collect telemetry
        tel = self.telemetry.collect()
        
        # Get AI prediction
        allocation = self.predictor.predict(tel)
        
        # Apply allocation
        self.controller.apply_allocation(allocation, tel)
        
        # Calculate reward (throughput proxy)
        reward = self._calculate_reward(tel, allocation)
        
        # Train model
        self.predictor.train(tel, reward)
        
        # Log status
        self.step += 1
        if self.step % 10 == 0:
            logger.info(
                f"Step {self.step} | "
                f"CPU: P={tel['cpu_p_util']:.1%} E={tel['cpu_e_util']:.1%} | "
                f"GPU: {tel.get('gpu_util', 0):.1%} Mem={tel.get('gpu_mem_pct', 0):.1%} | "
                f"RAM: {tel['mem_pressure']:.1%} | "
                f"Alloc: CPU={allocation['mining_cpu_affinity']} GPU={allocation['gpu_intensity']} | "
                f"Reward: {reward:.3f}"
            )
    
    def _calculate_reward(self, telemetry: Dict, allocation: Dict) -> float:
        """Calculate reward signal for learning."""
        # Reward = mining throughput proxy - resource contention penalty
        gpu_util = telemetry.get("gpu_util", 0)
        cpu_p = telemetry.get("cpu_p_util", 0)
        cpu_e = telemetry.get("cpu_e_util", 0)
        mem_pressure = telemetry.get("mem_pressure", 0)
        swap_pressure = telemetry.get("swap_used_pct", 0)
        
        # Positive: GPU utilization (mining), CPU utilization on correct cores
        throughput = gpu_util * 0.5 + (cpu_p if allocation["mining_cpu_affinity"] == "p_cores" else cpu_e) * 0.3
        
        # Negative: memory pressure, swap usage, thermal throttling
        penalty = mem_pressure * 0.5 + swap_pressure * 0.3
        if telemetry.get("gpu_temp_c", 0) > 80:
            penalty += 0.3
            
        return throughput - penalty

# ─── Entry Point ───────────────────────────────────────────────
def main():
    import argparse
    parser = argparse.ArgumentParser(description="AI Resource Allocation Optimizer")
    parser.add_argument("--daemon", action="store_true", help="Run as daemon")
    parser.add_argument("--once", action="store_true", help="Run single optimization step")
    parser.add_argument("--config", type=str, help="Config file path")
    args = parser.parse_args()
    
    # Load custom config if provided
    config = CONFIG.copy()
    if args.config and Path(args.config).exists():
        with open(args.config) as f:
            config.update(json.load(f))
    
    optimizer = AIResourceOptimizer(config)
    
    if args.once:
        tel = optimizer.telemetry.collect()
        allocation = optimizer.predictor.predict(tel)
        optimizer.controller.apply_allocation(allocation, tel)
        reward = optimizer._calculate_reward(tel, allocation)
        print(json.dumps({
            "telemetry": tel,
            "allocation": allocation,
            "reward": reward
        }, indent=2))
    else:
        try:
            optimizer.start()
        except KeyboardInterrupt:
            optimizer.stop()

if __name__ == "__main__":
    main()
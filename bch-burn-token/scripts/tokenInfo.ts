import { CONFIG } from "../lib/tokenConfig.js";

console.log("\n=== BURN Token ===");
console.log("Name:        BURN");
console.log("Ticker:      BURN");
console.log("Category:   ", process.env.BURN_CATEGORY || "(not set)");
console.log("Network:    ", process.env.NETWORK || CONFIG.NETWORK);
console.log("Mint Rate:  ", CONFIG.MINT_RATE, "BURN/sat");
console.log("Target:      ~$0.01 per BURN");
console.log("Standard:    CashTokens (CHIP-2)");
console.log("Supply:      Infinite (mineable)");
console.log("Contract:    contracts/BURN.cash (BurnMint)");

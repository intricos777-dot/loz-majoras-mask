import { readFileSync } from "fs";
import {
  Contract,
  ElectrumNetworkProvider,
  TransactionBuilder,
  SignatureTemplate,
} from "cashscript";
import { hexToBin, binToHex } from "@bitauth/libauth";
import { hash160 } from "@cashscript/utils";
import { CONFIG } from "../lib/tokenConfig.js";

interface FaucetConfig {
  network: string;
  tokenCategory: string;
  contractAddress: string;
  operatorPrivateKey: string;
  minBurnAmountBCH: number;
  maxBurnAmountBCH: number;
  cooldownMinutes: number;
  claimFeeBCH: number;
}

const DEFAULT_CONFIG: FaucetConfig = {
  network: "mainnet",
  tokenCategory: "",
  contractAddress: "",
  operatorPrivateKey: "",
  minBurnAmountBCH: 0.0001,
  maxBurnAmountBCH: 0.01,
  cooldownMinutes: 60,
  claimFeeBCH: 0.00001,
};

async function getConfig(): Promise<FaucetConfig> {
  const network = process.env.NETWORK || CONFIG.NETWORK;
  const tokenCategory = process.env.BURN_CATEGORY;
  const contractAddress = process.env.CONTRACT_ADDRESS;
  const operatorPrivateKey = process.env.OPERATOR_PRIVATE_KEY;

  if (!tokenCategory || !contractAddress || !operatorPrivateKey) {
    console.error("Required env vars: BURN_CATEGORY, CONTRACT_ADDRESS, OPERATOR_PRIVATE_KEY");
    process.exit(1);
  }

  return {
    ...DEFAULT_CONFIG,
    network,
    tokenCategory,
    contractAddress,
    operatorPrivateKey,
    minBurnAmountBCH: parseFloat(process.env.MIN_BURN_BCH || "0.0001"),
    maxBurnAmountBCH: parseFloat(process.env.MAX_BURN_BCH || "0.01"),
    cooldownMinutes: parseInt(process.env.COOLDOWN_MINUTES || "60"),
    claimFeeBCH: parseFloat(process.env.CLAIM_FEE_BCH || "0.00001"),
  };
}

async function main() {
  const config = await getConfig();
  const provider = new ElectrumNetworkProvider(config.network);

  const privateKey = hexToBin(config.operatorPrivateKey);
  const sigTemplate = new SignatureTemplate(privateKey);
  const pubkey = sigTemplate.getPublicKey();
  const pkh = binToHex(hash160(pubkey));

  const { encodeCashAddress, CashAddressNetworkPrefix, CashAddressType }: any =
    await import("@bitauth/libauth");
  const operatorAddress = encodeCashAddress({
    payload: pkh,
    prefix:
      config.network === "mainnet"
        ? CashAddressNetworkPrefix.mainnet
        : CashAddressNetworkPrefix.chipnet,
    type: CashAddressType.p2pkh,
  }).address;

  console.log("=== BURN Token Faucet ===");
  console.log("Operator:", operatorAddress);
  console.log("Network:", config.network);
  console.log("Contract:", config.contractAddress);
  console.log("Token ID:", config.tokenCategory);
  console.log("");

  const artifact = JSON.parse(readFileSync("artifacts/BURN.json", "utf-8"));
  const contract = new Contract(artifact, [], { provider, address: config.contractAddress });

  const operatorUtxos = await provider.getUtxos(operatorAddress);
  if (operatorUtxos.length === 0) {
    console.error("Operator wallet empty. Fund with BCH first.");
    process.exit(1);
  }

  const contractUtxos = await contract.getUtxos();
  const mintBatonUtxo = contractUtxos.find(
    (u: any) => u.token?.category === config.tokenCategory && u.token?.nft?.capability === "minting",
  );

  if (!mintBatonUtxo) {
    console.error("No mint baton in contract. Deploy & fund first.");
    process.exit(1);
  }

  console.log("Mint baton found:", mintBatonUtxo.txid, "vout:", mintBatonUtxo.vout);
  console.log("Contract balance:", mintBatonUtxo.satoshis, "sat");
  console.log("");

  const amountArg = process.argv[2];
  if (!amountArg) {
    console.log("Usage:");
    console.log("  npx tsx scripts/faucet.ts claim <bch_amount>  # Burn BCH, get BURN");
    console.log("  npx tsx scripts/faucet.ts balance             # Check balances");
    console.log("  npx tsx scripts/faucet.ts stats               # Show faucet stats");
    console.log("");
    console.log("Env vars:");
    console.log("  BURN_CATEGORY, CONTRACT_ADDRESS, OPERATOR_PRIVATE_KEY");
    console.log("  MIN_BURN_BCH=0.0001, MAX_BURN_BCH=0.01, COOLDOWN_MINUTES=60");
    process.exit(0);
  }

  const command = amountArg;

  if (command === "balance") {
    const balance = operatorUtxos.reduce((s, u) => s + BigInt(u.satoshis), 0n);
    console.log("Operator BCH:", Number(balance) / 100_000_000, "BCH");

    const tokenUtxos = operatorUtxos.filter(u => u.token?.category === config.tokenCategory);
    const totalBurn = tokenUtxos.reduce((s, u) => s + (u.token?.amount ? BigInt(u.token.amount) : 0n), 0n);
    console.log("Operator BURN:", totalBurn.toString());
    return;
  }

  if (command === "stats") {
    console.log("Faucet Config:");
    console.log("  Min burn:", config.minBurnAmountBCH, "BCH");
    console.log("  Max burn:", config.maxBurnAmountBCH, "BCH");
    console.log("  Cooldown:", config.cooldownMinutes, "min");
    console.log("  Claim fee:", config.claimFeeBCH, "BCH");
    console.log("");
    console.log("Contract:");
    console.log("  Address:", config.contractAddress);
    console.log("  Mint baton UTXO:", mintBatonUtxo.satoshis, "sat");
    return;
  }

  const bchAmount = parseFloat(command);
  if (isNaN(bchAmount) || bchAmount < config.minBurnAmountBCH || bchAmount > config.maxBurnAmountBCH) {
    console.error(`Amount must be between ${config.minBurnAmountBCH} and ${config.maxBurnAmountBCH} BCH`);
    process.exit(1);
  }

  const satoshis = BigInt(Math.floor(bchAmount * 100_000_000));
  const burnAmount = satoshis * BigInt(CONFIG.MINT_RATE);
  const feeSats = BigInt(Math.floor(config.claimFeeBCH * 100_000_000));

  console.log(`Burning: ${bchAmount} BCH (${satoshis} sat)`);
  console.log(`Minting: ${burnAmount.toString()} BURN tokens`);
  console.log(`Fee:     ${config.claimFeeBCH} BCH`);

  const feeUtxo = operatorUtxos[0];
  const totalInput = (inputs: any[]) => inputs.reduce((s, i) => s + BigInt(i.satoshis), 0n);
  const totalOutput = (outputs: any[]) => outputs.reduce((s, o) => s + BigInt(o.amount || 0), 0n);

  try {
    const inputs = [mintBatonUtxo, feeUtxo];
    const out0 = {
      to: config.contractAddress,
      amount: 546n,
      token: { amount: 0n, category: config.tokenCategory, nft: { capability: "minting" as const, commitment: "00".repeat(32) } }
    };
    const out1 = {
      to: operatorAddress,
      amount: 546n,
      token: { amount: burnAmount, category: config.tokenCategory }
    };
    const fee = 50000n;
    const change = totalInput(inputs) - totalOutput([out0, out1]) - fee - feeSats;

    if (change < 0n) {
      console.error("Insufficient funds for fee + change");
      process.exit(1);
    }

    console.log("\nBroadcasting transaction...");
    const tx = await new TransactionBuilder({ provider })
      .addInput(mintBatonUtxo, contract.unlock.mint())
      .addInput(feeUtxo, sigTemplate.unlockP2PKH())
      .addOutput(out0)
      .addOutput(out1)
      .addOpReturnOutput(["0x4249524E", command])
      .addOutput({ to: operatorAddress, amount: change })
      .setMaxFee(fee + feeSats)
      .send();

    console.log(`\n✅ MINT SUCCESSFUL`);
    console.log(`TXID: ${tx.txid}`);
    console.log(`Minted: ${burnAmount.toString()} BURN`);
    console.log(`Explorer: https://explorer.bitcoin.com/bch/tx/${tx.txid}`);
  } catch (err: any) {
    console.error("Mint failed:", err.message || err);
    process.exit(1);
  }
}

main().catch(console.error);
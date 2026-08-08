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

/**
 * BURN Token Deployment
 * Usage: export PRIVATE_KEY=<64-hex> && npx tsx scripts/deploy.ts
 */

async function main() {
  const network: any = process.env.NETWORK || CONFIG.NETWORK;
  const provider = new ElectrumNetworkProvider(network);

  const privKeyHex = process.env.PRIVATE_KEY;
  if (!privKeyHex || privKeyHex.length !== 64) {
    console.error("Set PRIVATE_KEY env var (64 hex chars)");
    process.exit(1);
  }
  const privateKey = hexToBin(privKeyHex);
  const sigTemplate = new SignatureTemplate(privateKey);
  const pubkey = sigTemplate.getPublicKey();
  const pkh = binToHex(hash160(pubkey));

  const { encodeCashAddress, CashAddressNetworkPrefix, CashAddressType }: any =
    await import("@bitauth/libauth");
  const deployerAddress = encodeCashAddress({
    payload: pkh,
    prefix:
      network === "mainnet"
        ? CashAddressNetworkPrefix.mainnet
        : CashAddressNetworkPrefix.chipnet,
    type: CashAddressType.p2pkh,
  }).address;

  console.log("Deployer:", deployerAddress);

  const utxos: any[] = await provider.getUtxos(deployerAddress);
  if (utxos.length === 0) {
    console.error("Fund your address with BCH first");
    process.exit(1);
  }
  const balance = utxos.reduce((s: bigint, u: any) => s + BigInt(u.satoshis), 0n);
  console.log("Balance:", balance.toString(), "sat");

  // === Compute change helper ===
  const totalInput = (inputs: any[]) =>
    inputs.reduce((s: bigint, i: any) => s + BigInt(i.satoshis), 0n);
  const totalOutput = (outputs: any[]) =>
    outputs.reduce((s: bigint, o: any) => s + BigInt(o.amount || 0), 0n);

  // === Step 1: Genesis ===
  console.log("\n=== Genesis ===");
  const GENESIS_PLACEHOLDER =
    "0000000000000000000000000000000000000000000000000000000000000000";
  const initialSupply = 100000000n;

  const genesisFeeInput = utxos[0];
  const genInputs = [genesisFeeInput];
  const genOutV0 = { to: deployerAddress, amount: 546n, token: { amount: initialSupply, category: GENESIS_PLACEHOLDER, nft: { capability: "minting" as const, commitment: "00".repeat(32) } } };
  const genOutV1 = { to: deployerAddress, amount: 546n, token: { amount: 0n, category: GENESIS_PLACEHOLDER } };
  const genChange = totalInput(genInputs) - totalOutput([genOutV0, genOutV1]) - 50000n;
  const genOutChange = { to: deployerAddress, amount: genChange };

  const genesisTx = await new TransactionBuilder({ provider })
    .addInput(genesisFeeInput, sigTemplate.unlockP2PKH())
    .addOutput(genOutV0)
    .addOutput(genOutV1)
    .addOutput(genOutChange)
    .setMaxFee(50000n)
    .send();

  console.log("Genesis TX:", genesisTx.txid);
  await new Promise((r) => setTimeout(r, 4000));

  // === Step 2: Read category ===
  console.log("\n=== Reading category ===");
  const updatedUtxos: any[] = await provider.getUtxos(deployerAddress);
  const tokenOutput = updatedUtxos.find(
    (u: any) => u.token && u.txid === genesisTx.txid && u.token.amount > 0n,
  );
  if (!tokenOutput?.token) {
    console.error("Genesis output not found");
    process.exit(1);
  }
  const tokenCategory = tokenOutput.token.category;
  console.log("Token Category:", tokenCategory);

  const mintBatonUtxo = updatedUtxos.find(
    (u: any) => u.token?.category === tokenCategory && u.token?.nft?.capability === "minting",
  );
  if (!mintBatonUtxo) {
    console.error("Mint baton not found");
    process.exit(1);
  }

  // === Step 3: Fund contract ===
  console.log("\n=== Funding contract ===");
  const artifact = JSON.parse(readFileSync("artifacts/BURN.json", "utf-8"));
  const contract = new Contract(artifact, [], { provider });
  console.log("Contract Address:", contract.address);

  const feeUtxo = updatedUtxos.find((u: any) => u.txid !== mintBatonUtxo.txid || u.vout !== mintBatonUtxo.vout) || updatedUtxos[0];
  const fundInputs = [mintBatonUtxo, feeUtxo];
  const fundOut0 = { to: contract.address, amount: 10000n, token: { amount: 0n, category: tokenCategory, nft: { capability: "minting" as const, commitment: "00".repeat(32) } } };
  const fundChange = totalInput(fundInputs) - totalOutput([fundOut0]) - 50000n;
  const fundOutChange = { to: deployerAddress, amount: fundChange };

  const fundTx = await new TransactionBuilder({ provider })
    .addInputs(fundInputs, sigTemplate.unlockP2PKH())
    .addOutput(fundOut0)
    .addOutput(fundOutChange)
    .setMaxFee(50000n)
    .send();

  console.log("Contract Funded TX:", fundTx.txid);

  console.log("\n" + "=".repeat(52));
  console.log("  BURN Token Deployed!");
  console.log("=".repeat(52));
  console.log(`  Token Category: ${tokenCategory}`);
  console.log(`  Contract Addr:  ${contract.address}`);
  console.log(`  Initial Supply: ${initialSupply.toString()} BURN`);
  console.log("=".repeat(52));
  console.log(`\n  BURN_CATEGORY=${tokenCategory}\n  CONTRACT_ADDRESS=${contract.address}`);
}

main().catch(console.error);

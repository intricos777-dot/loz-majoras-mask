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
 * BURN Token Mining
 * Usage: export PRIVATE_KEY=<64-hex> BURN_CATEGORY=<id> CONTRACT_ADDRESS=<addr>
 *        npx tsx scripts/mint.ts <bch-amount>
 */

async function main() {
  const network: any = process.env.NETWORK || CONFIG.NETWORK;
  const provider = new ElectrumNetworkProvider(network);

  const bchArg = process.argv[2];
  if (!bchArg) {
    console.error("Usage: npx tsx scripts/mint.ts <amount_in_BCH>");
    process.exit(1);
  }
  const bchAmount = parseFloat(bchArg);
  if (isNaN(bchAmount) || bchAmount <= 0) {
    console.error("Invalid amount");
    process.exit(1);
  }
  const satoshis = BigInt(Math.floor(bchAmount * 100_000_000));

  const tokenCategory = process.env.BURN_CATEGORY;
  const contractAddress = process.env.CONTRACT_ADDRESS;
  if (!tokenCategory || !contractAddress) {
    console.error("Set BURN_CATEGORY and CONTRACT_ADDRESS env vars");
    process.exit(1);
  }

  const privKeyHex = process.env.PRIVATE_KEY;
  if (!privKeyHex) {
    console.error("Set PRIVATE_KEY env var");
    process.exit(1);
  }
  const privateKey = hexToBin(privKeyHex);
  const sigTemplate = new SignatureTemplate(privateKey);
  const pubkey = sigTemplate.getPublicKey();
  const pkh = binToHex(hash160(pubkey));

  const { encodeCashAddress, CashAddressNetworkPrefix, CashAddressType }: any =
    await import("@bitauth/libauth");
  const minerAddress = encodeCashAddress({
    payload: pkh,
    prefix: network === "mainnet" ? CashAddressNetworkPrefix.mainnet : CashAddressNetworkPrefix.chipnet,
    type: CashAddressType.p2pkh,
  }).address;
  console.log("Miner:", minerAddress);

  const artifact = JSON.parse(readFileSync("artifacts/BURN.json", "utf-8"));
  const contract = new Contract(artifact, [], { provider });

  const contractUtxos: any[] = await contract.getUtxos();
  const mintBatonUtxo = contractUtxos.find(
    (u: any) => u.token?.category === tokenCategory && u.token?.nft?.capability === "minting",
  );
  if (!mintBatonUtxo) {
    console.error("No mint baton found. Deploy first.");
    process.exit(1);
  }
  console.log("Contract UTXO:", mintBatonUtxo.satoshis.toString(), "sat");

  const burnAmount = satoshis * BigInt(CONFIG.MINT_RATE);
  console.log(`Minting ${burnAmount.toString()} BURN`);

  const minerUtxos: any[] = await provider.getUtxos(minerAddress);
  const feeUtxo = minerUtxos[0];
  if (!feeUtxo) {
    console.error("Fund your wallet first");
    process.exit(1);
  }

  const totalInput = (inputs: any[]) =>
    inputs.reduce((s: bigint, i: any) => s + BigInt(i.satoshis), 0n);
  const totalOutput = (outputs: any[]) =>
    outputs.reduce((s: bigint, o: any) => s + BigInt(o.amount || 0), 0n);

  try {
    const inputs = [mintBatonUtxo, feeUtxo];
    const out0 = { to: contract.address, amount: 546n, token: { amount: 0n, category: tokenCategory, nft: { capability: "minting" as const, commitment: "00".repeat(32) } } };
    const out1 = { to: minerAddress, amount: 546n, token: { amount: burnAmount, category: tokenCategory } };
    const fee = 50000n;
    const change = totalInput(inputs) - totalOutput([out0, out1]) - fee;

    const tx = await new TransactionBuilder({ provider })
      .addInput(mintBatonUtxo, contract.unlock.mint())
      .addInput(feeUtxo, sigTemplate.unlockP2PKH())
      .addOutput(out0)
      .addOutput(out1)
      .addOpReturnOutput(["0x4249524E", bchArg])
      .addOutput({ to: minerAddress, amount: change })
      .setMaxFee(fee)
      .send();

    console.log(`\nMINT SUCCESSFUL — TXID: ${tx.txid}`);
    console.log(`Minted ${burnAmount.toString()} BURN`);
    console.log(`Explorer: https://explorer.bitcoin.com/bch/tx/${tx.txid}`);
  } catch (err: any) {
    console.error("Mint failed:", err.message || err);
    process.exit(1);
  }
}

main().catch(console.error);

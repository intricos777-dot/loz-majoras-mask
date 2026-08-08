# BURN Token — Mineable Fungible Token on Bitcoin Cash

A fungible token on the Bitcoin Cash network, mined by **proof-of-burn**.

| Property      | Value                                                |
|---------------|------------------------------------------------------|
| Name          | BURN                                                 |
| Ticker        | BURN                                                 |
| Pseudonym     | BURN                                                 |
| Standard      | CashTokens (CHIP-2) — native BCH fungible tokens     |
| Supply        | Infinite (mineable forever)                          |
| Mining        | Burn BCH → mint BURN                                 |
| Target Price  | ~1 US cent per BURN (adjustable mint rate)           |
| Fungible      | Yes — native BCH token standard                      |

## How It Works

1. **Deployer** creates the BURN token category and locks the mint baton in a public smart contract
2. **Anyone** can send BCH to the contract address to "burn" it (provably destroy it)
3. The **contract mints** new BURN tokens proportional to the BCH burned
4. The **mint baton stays in the contract**, so mining can continue indefinitely
5. BURN tokens can be **sent, traded, and used** like any BCH fungible token

### Mint Rate

Targeting ~1¢ per BURN requires adjusting the mint rate as BCH price changes:

```
BCH price = $P
1 sat = $P / 100,000,000
BURN per sat = $0.01 / ($P / 100,000,000) = 1,000,000 / P
```

At $350/BCH: ~2,857 BURN per sat → 1 sat burned = 2,857 BURN ≈ 1¢

The initial `MINT_RATE` in `lib/tokenConfig.ts` is set to **2857**.

> ⚠️ **No on‑chain mechanism can permanently enforce a fiat peg.**
> This ratio sets the minting cost — secondary market price is determined by supply/demand.

## Getting Started

### Prerequisites

- Node.js >= 18
- A BCH wallet with a mnemonic seed phrase (12/24 words)
- BCH for deployment and mining fees

### Install

```bash
git clone <repo-url> bch-burn-token
cd bch-burn-token
npm install
```

### Deploy

```bash
export MNEMONIC="your twelve word mnemonic phrase"
npx tsx scripts/deploy.ts
```

Save the output values:
- `TOKEN CATEGORY`
- `CONTRACT ADDRESS`

### Mine BURN Tokens

```bash
export MNEMONIC="your twelve word mnemonic phrase"
export BURN_CATEGORY="<token category from deploy>"
export CONTRACT_ADDRESS="<contract address from deploy>"

# Burn 0.001 BCH to mine BURN
npx tsx scripts/mint.ts 0.001
```

### Check Token Info

```bash
export BURN_CATEGORY="<token category from deploy>"
npx tsx scripts/tokenInfo.ts
```

## Project Structure

```
bch-burn-token/
├── contracts/
│   └── BURN.cash         # CashScript smart contract
├── scripts/
│   ├── deploy.ts         # Genesis & contract deployment
│   ├── mint.ts           # Mine BURN by burning BCH
│   └── tokenInfo.ts      # Query token metadata
├── lib/
│   └── tokenConfig.ts    # Mint rate, network, metadata
├── package.json
└── README.md
```

## Contract Design

The `BurnMint` contract enforces:

1. **BCH > 0**: The UTXO must carry value to burn
2. **Burn proof**: At least one output sends value to an unspendable script
3. **Token mint**: New BURN tokens are minted proportional to satoshis burned
4. **Mint baton permanence**: The contract returns the mint baton to itself, keeping mining open forever

## Use Cases

- **Permissionless mining**: Anyone with BCH can mine BURN
- **Proof-of-burn games**: Use BURN as a verifiable "burn credit"
- **Deflationary sink**: BCH is provably destroyed in exchange for BURN
- **Community currency**: Fixed mining cost creates a natural floor

## Cross-Chain Burn Distribution

The initial request included distributing to burners on all crypto networks (past and future). This is not feasible in a trustless on-chain manner without:

- Cross-chain oracle infrastructure (e.g., zk-light clients)
- Centralized indexer with attested proofs
- Manual airdrop claim mechanism

The current implementation supports **BCH native proof-of-burn**. For cross-chain burns, a future version could add:

- A web dashboard where users submit burn proofs (TXIDs from other chains)
- A multisig or DAO to verify and distribute BURN
- Integration with LayerZero / zkBridge for trustless cross-chain attestation

## License

MIT

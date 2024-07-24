/// Training parameters used to create Renegade's networks
/// bullet version used: 29d815e from July 12, 2024

use bullet_lib::{
    inputs, outputs, lr, wdl, Activation, LocalSettings,
    TrainerBuilder, TrainingSchedule, Loss, optimiser
};

macro_rules! net_id {
    () => {
        "renegade-net-24"
    };
}
const NET_ID: &str = net_id!();


fn main() {
    let mut trainer = TrainerBuilder::default()
	    .quantisations(&[255, 64])
        .optimiser(optimiser::AdamW)
        .input(inputs::ChessBucketsMirrored::new([
            0, 0, 1, 1,
            2, 2, 2, 2,
            2, 2, 2, 2,
            3, 3, 3, 3,
            3, 3, 3, 3,
            3, 3, 3, 3,
            3, 3, 3, 3,
            3, 3, 3, 3,
        ]))
        .output_buckets(outputs::Single)
        .feature_transformer(1024)
        .activate(Activation::SCReLU)
        .add_layer(1)
        .build();  //trainer.load_from_checkpoint("checkpoints/testnet");
	
    let schedule = TrainingSchedule {
        net_id: NET_ID.to_string(),
        batch_size: 16384,
        eval_scale: 400.0,
        ft_regularisation: 0.0,
        batches_per_superbatch: 6104,  // ~100 million positions
        start_superbatch: 1,
        end_superbatch: 520,
        wdl_scheduler: wdl::LinearWDL {
            start: 0.2,
            end: 0.4,
        },
        lr_scheduler: lr::StepLR {
            start: 0.001,
            gamma: 0.3,
            step: 120,
        },
        loss_function: Loss::SigmoidMSE,
        save_rate: 40,
        optimiser_settings: optimiser::AdamWParams {
            decay: 0.01,
            beta1: 0.9,
            beta2: 0.999,
            min_weight: -1.98,
            max_weight: 1.98,
        },
    };
	
	
    let settings = LocalSettings {
        threads: 6,
        data_file_paths: vec![
            "../nnue/data/240325_240421_240609_240622_240722_frc240513",
        ],
        test_set: None,
        output_directory: "checkpoints",
    };

    trainer.run(&schedule, &settings);
	
    for fen in [
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        "rn3r2/pbppq1p1/1p2pN1k/4N3/3P4/3B4/PPP2PPP/R3K2R w KQ - 1 13",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "2r3k1/2P2pp1/3Np2p/8/7P/5qP1/5P1K/2Q5 b - - 2 42",
        "rnbqkb1r/pppppppp/8/3nP3/2P5/8/PP1P1PPP/RNBQKBNR b KQkq - 0 3",
        "2rk1b1r/1Qp1p2p/p1P5/5p2/2PP2B1/4B3/5P1P/1R2K3 w - - 0 35",
        "rnbqkb1r/pp2pppp/2p2n2/3p4/2PP4/5N2/PP2PPPP/RNBQKB1R w KQkq - 0 4",
        "r3r1k1/ppp2ppp/3qbb2/2p1N2Q/5P2/1PPB4/P1P3PP/1R3RK1 b - - 2 15",
        "7k/pp4rp/3p1Q2/1P1P4/2Pp4/3Pb2P/2q3P1/5R1K w - - 7 37",
        "8/8/4p3/4P1p1/Pk6/2p5/1p3K2/1r6 b - - 1 52",
        "r2qr1k1/pp2bppp/2n2n2/5b2/2P1N3/3P1N2/PP1BQPPP/R3KB1R w KQ - 2 11",
        "r6k/p1nP3p/n1QP1qp1/1B6/1b6/7P/2P2PP1/1R1R2K1 b - - 0 32",
        "1n3rk1/7p/1q4p1/p1p2pR1/2B2P2/4P3/P1QP1P2/4K3 b - - 1 24",
        "2r4k/5Qp1/p6p/3Np1b1/4P3/P7/1P2n1PP/2r2R1K w - - 1 33",
        "3r4/5p1k/5bpP/8/4K1P1/2p1PN2/8/7R b - - 9 105",
        "r1bqk2r/p1p1bppp/1p2pB2/8/3P4/3B1N2/PPP2PPP/R2QK2R b KQkq - 0 9",
        "1rbqk2r/4ppb1/2np2p1/p1p4p/N1P1P3/4BP1P/PP1Q2P1/2KR1B1R w k - 1 15",
        "5k2/8/4p2p/4P1p1/3P2P1/3b3K/3B2P1/8 w - - 92 121",
        "rnbqk2r/ppppppbp/5np1/8/2PP4/2N2N2/PP2PPPP/R1BQKB1R b KQkq - 3 4",
        "2r1r1k1/p3B2p/3p1P2/2pP3q/P1P3n1/1R3p1P/2Q3P1/1R5K b - - 2 47",
        "8/8/6R1/5K1p/8/5k2/6p1/8 w - - 0 79",
        "3k4/8/1Q6/1b6/8/1P1p1P2/3K4/7q b - - 7 64",
        "6Q1/8/8/7k/8/8/3p1pp1/3Kbrrb w - - 0 1",
        "1rqbkrbn/1ppppp1p/1n6/p1N3p1/8/2P4P/PP1PPPP1/1RQBKRBN w FBfb - 0 9",
        "rbbqn1kr/pp2p1pp/6n1/2pp1p2/2P4P/P7/BP1PPPP1/R1BQNNKR w HAha - 0 9",
        "rqbbk1r1/1ppp2pp/p3n1n1/4pp2/P7/1PP1N3/1Q1PPPPP/R1BB1RKN b ga - 3 10",
        "brnqnbkr/pppppppp/8/8/8/8/PPPPPPPP/BQNRNKRB w GDhb - 0 1"
    ] {
        let eval = trainer.eval(fen);
        println!("FEN: {} -> eval: {}", fen, 400.0 * eval);
    }
}

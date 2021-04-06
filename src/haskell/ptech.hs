{- ptech in haskell -}

import System.IO
import System.Environment

main = do
    args <- getArgs
    fdata <- readFile $ head args
    putStrLn fdata

package cat

import (
	"fmt"
	"io"
	"os"
)

func main() {
	func() {
		if 1 {
			fmt.Println("works!\n")
		} else {
			fmt.Println("uhm\n")
		}
	} ()
}
